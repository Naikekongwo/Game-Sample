#ifndef THREAD_MANAGER_HPP
#define THREAD_MANAGER_HPP

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

enum class TaskPriority
{
    LOW, // 默认优先级
    NORMAL,
    HIGH,
    CRITICAL
};

class ThreadManager
{
  public:
    static ThreadManager &getInstance();

    ThreadManager(const ThreadManager &) = delete;
    ThreadManager &operator=(const ThreadManager &) = delete;

    // 启动线程池，可指定最小、最大线程数
    void start(size_t minThreads = 1,
               size_t maxThreads = std::thread::hardware_concurrency());

    // 停止线程池
    void shutdown();

    // 手动设置线程数（会覆盖自动调节）
    void set_thread_count(size_t newCount);

    // 启用/禁用自动调节
    void enable_auto_adjust(bool enable);

    // 提交任务到工作线程池（默认低优先级）
    template <typename F, typename... Args>
    auto submit(F &&f, Args &&...args)
        -> std::future<std::invoke_result_t<F, Args...>>;

    // 提交指定优先级的任务
    template <typename F, typename... Args>
    auto submit_priority(TaskPriority priority, F &&f, Args &&...args)
        -> std::future<std::invoke_result_t<F, Args...>>;

    // 提交需要在主线程执行的任务
    void submit_to_main_thread(std::function<void()> task);

    // 主线程调用此函数处理所有待处理的主线程任务
    void process_main_thread_tasks();

    // 等待所有已提交的任务（包括主线程任务）完成
    void wait_for_all_tasks();

    // 获取当前活跃任务数（工作线程 + 主线程任务）
    size_t active_task_count() const;

  private:
    ThreadManager() = default;
    ~ThreadManager();

    // 内部任务结构（无取消相关字段）
    struct Task
    {
        TaskPriority priority;
        std::function<void()> func;

        bool operator<(const Task &other) const
        {
            return static_cast<int>(priority) <
                   static_cast<int>(other.priority);
        }
    };

    void worker_loop(size_t index);
    void monitor_loop();
    size_t calculate_target_thread_count();

    // 工作线程相关
    std::vector<std::thread> workers_;
    std::priority_queue<Task> task_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::atomic<bool> stop_{false};
    std::atomic<size_t> target_thread_count_{0};
    std::atomic<size_t> active_worker_tasks_{0};

    // 主线程任务队列
    std::mutex main_queue_mutex_;
    std::queue<std::function<void()>> main_tasks_;
    std::atomic<size_t> active_main_tasks_{0};

    // 自动调节相关
    std::thread monitor_thread_;
    std::mutex monitor_mutex_;
    std::condition_variable monitor_cv_;
    std::atomic<bool> monitor_stop_{false};
    std::atomic<bool> auto_adjust_enabled_{true};
    size_t min_threads_{1};
    size_t max_threads_{std::thread::hardware_concurrency()};

    static constexpr std::chrono::milliseconds ADJUST_INTERVAL{1000};
};

// 模板方法实现
template <typename F, typename... Args>
auto ThreadManager::submit(F &&f, Args &&...args)
    -> std::future<std::invoke_result_t<F, Args...>>
{
    return submit_priority(TaskPriority::LOW, std::forward<F>(f),
                           std::forward<Args>(args)...);
}

template <typename F, typename... Args>
auto ThreadManager::submit_priority(TaskPriority priority, F &&f,
                                    Args &&...args)
    -> std::future<std::invoke_result_t<F, Args...>>
{
    using return_type = typename std::invoke_result_t<F, Args...>;

    auto packaged = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    auto future = packaged->get_future();

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        task_queue_.emplace(Task{priority, [packaged] { (*packaged)(); }});
        active_worker_tasks_++;
    }
    queue_cv_.notify_one();
    return future;
}

#endif // THREAD_MANAGER_HPP