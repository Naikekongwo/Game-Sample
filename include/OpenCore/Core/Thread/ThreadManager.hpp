/**
 * @file ThreadManager.hpp
 * @brief 线程池管理器模块。
 * 
 * 提供动态线程池，支持任务优先级、主线程任务队列、自动线程数调节等功能。
 */

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

/**
 * @enum TaskPriority
 * @brief 任务优先级枚举。
 * 
 * 优先级从低到高：LOW, NORMAL, HIGH, CRITICAL。
 * 优先级高的任务会优先从队列中取出执行。
 */
enum class TaskPriority
{
    LOW,      ///< 低优先级（默认）
    NORMAL,   ///< 普通优先级
    HIGH,     ///< 高优先级
    CRITICAL  ///< 关键优先级
};

/**
 * @class ThreadManager
 * @brief 线程池管理器单例类。
 * 
 * 管理一个动态线程池，支持：
 * - 提交具有优先级的异步任务
 * - 提交需要在主线程执行的任务
 * - 自动或手动调整线程数量
 * - 等待所有任务完成
 */
class ThreadManager
{
public:
    /**
     * @brief 获取 ThreadManager 单例实例。
     * @return ThreadManager 的引用。
     */
    static ThreadManager &getInstance();

    ThreadManager(const ThreadManager &) = delete;
    ThreadManager &operator=(const ThreadManager &) = delete;

    /**
     * @brief 启动线程池。
     * @param minThreads 最小线程数（默认为1）。
     * @param maxThreads 最大线程数（默认为硬件并发数）。
     */
    void start(size_t minThreads = 1,
               size_t maxThreads = std::thread::hardware_concurrency());

    /**
     * @brief 停止线程池，等待所有线程结束并清空任务队列。
     */
    void shutdown();

    /**
     * @brief 手动设置线程池中的线程数量（会禁用自动调节）。
     * @param newCount 新的线程数量，将被限制在 [min_threads_, max_threads_] 范围内。
     */
    void set_thread_count(size_t newCount);

    /**
     * @brief 启用或禁用自动线程数调节。
     * @param enable true 启用，false 禁用。
     */
    void enable_auto_adjust(bool enable);

    /**
     * @brief 提交一个任务到工作线程池（默认低优先级）。
     * @tparam F 可调用对象类型。
     * @tparam Args 参数类型包。
     * @param f 可调用对象。
     * @param args 传递给 f 的参数。
     * @return std::future 用于获取任务返回值。
     */
    template <typename F, typename... Args>
    auto submit(F &&f, Args &&...args)
        -> std::future<std::invoke_result_t<F, Args...>>;

    /**
     * @brief 提交指定优先级的任务到工作线程池。
     * @tparam F 可调用对象类型。
     * @tparam Args 参数类型包。
     * @param priority 任务优先级。
     * @param f 可调用对象。
     * @param args 传递给 f 的参数。
     * @return std::future 用于获取任务返回值。
     */
    template <typename F, typename... Args>
    auto submit_priority(TaskPriority priority, F &&f, Args &&...args)
        -> std::future<std::invoke_result_t<F, Args...>>;

    /**
     * @brief 提交一个需要在主线程执行的任务。
     * @param task 要执行的可调用对象（无返回值）。
     */
    void submit_to_main_thread(std::function<void()> task);

    /**
     * @brief 主线程调用此函数处理所有待处理的主线程任务。
     * 
     * 该函数应定期在主线程中调用，以确保提交到主线程的任务得到执行。
     */
    void process_main_thread_tasks();

    /**
     * @brief 等待所有已提交的任务（包括工作线程任务和主线程任务）完成。
     */
    void wait_for_all_tasks();

    /**
     * @brief 获取当前活跃任务数（工作线程中正在执行的任务 + 主线程队列中待处理的任务）。
     * @return 活跃任务数量。
     */
    size_t active_task_count() const;

private:
    ThreadManager() = default;
    ~ThreadManager();

    /**
     * @struct Task
     * @brief 内部任务结构，包含优先级和执行函数。
     */
    struct Task
    {
        TaskPriority priority;          ///< 任务优先级
        std::function<void()> func;     ///< 实际执行的任务函数

        /**
         * @brief 优先级比较运算符，用于优先队列（高优先级排在前面）。
         * @param other 另一个任务对象。
         * @return 若当前任务优先级低于 other 则返回 true。
         */
        bool operator<(const Task &other) const
        {
            return static_cast<int>(priority) <
                   static_cast<int>(other.priority);
        }
    };

    void worker_loop(size_t index);                     ///< 工作线程主循环
    void monitor_loop();                                ///< 监控线程循环（自动调节）
    size_t calculate_target_thread_count();             ///< 根据队列负载计算目标线程数

    // 工作线程相关
    std::vector<std::thread> workers_;                  ///< 工作线程列表
    std::priority_queue<Task> task_queue_;              ///< 任务优先队列
    std::mutex queue_mutex_;                            ///< 保护任务队列的互斥锁
    std::condition_variable queue_cv_;                  ///< 任务队列条件变量
    std::atomic<bool> stop_{false};                     ///< 停止标志
    std::atomic<size_t> target_thread_count_{0};        ///< 目标线程数量
    std::atomic<size_t> active_worker_tasks_{0};        ///< 工作线程中正在执行的任务计数

    // 主线程任务队列
    std::mutex main_queue_mutex_;                       ///< 保护主线程任务队列的互斥锁
    std::queue<std::function<void()>> main_tasks_;      ///< 主线程任务队列
    std::atomic<size_t> active_main_tasks_{0};          ///< 主线程队列中待处理的任务数

    // 自动调节相关
    std::thread monitor_thread_;                        ///< 监控线程
    std::mutex monitor_mutex_;                          ///< 保护监控线程条件变量的互斥锁
    std::condition_variable monitor_cv_;                ///< 监控线程条件变量
    std::atomic<bool> monitor_stop_{false};             ///< 监控线程停止标志
    std::atomic<bool> auto_adjust_enabled_{true};       ///< 自动调节是否启用
    size_t min_threads_{1};                             ///< 最小线程数
    size_t max_threads_{std::thread::hardware_concurrency()}; ///< 最大线程数

    static constexpr std::chrono::milliseconds ADJUST_INTERVAL{1000}; ///< 自动调节间隔（1秒）
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