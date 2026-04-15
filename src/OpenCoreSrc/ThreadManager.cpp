#include "Opencore/Core/Thread/ThreadManager.hpp"
#include <stdexcept>
#include <algorithm>

ThreadManager& ThreadManager::getInstance() {
    static ThreadManager manager;
    return manager;
}

ThreadManager::~ThreadManager() {
    shutdown();
}

void ThreadManager::start(size_t minThreads, size_t maxThreads) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    if (!workers_.empty()) return;

    min_threads_ = std::max<size_t>(1, minThreads);
    max_threads_ = std::max(min_threads_, maxThreads);
    target_thread_count_ = min_threads_;
    stop_ = false;
    monitor_stop_ = false;
    auto_adjust_enabled_ = true;

    for (size_t i = 0; i < target_thread_count_; ++i) {
        workers_.emplace_back(&ThreadManager::worker_loop, this, i);
    }

    monitor_thread_ = std::thread(&ThreadManager::monitor_loop, this);
}

void ThreadManager::shutdown() {
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }
    queue_cv_.notify_all();

    {
        std::lock_guard<std::mutex> lock(monitor_mutex_);
        monitor_stop_ = true;
    }
    monitor_cv_.notify_one();

    for (auto& t : workers_) {
        if (t.joinable()) t.join();
    }
    workers_.clear();

    if (monitor_thread_.joinable()) monitor_thread_.join();

    std::lock_guard<std::mutex> lock(main_queue_mutex_);
    while (!main_tasks_.empty()) main_tasks_.pop();
    active_main_tasks_ = 0;
}

void ThreadManager::set_thread_count(size_t newCount) {
    auto_adjust_enabled_ = false;
    newCount = std::clamp(newCount, min_threads_, max_threads_);
    size_t old = target_thread_count_.exchange(newCount);
    if (newCount > old) {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        for (size_t i = old; i < newCount; ++i) {
            workers_.emplace_back(&ThreadManager::worker_loop, this, i);
        }
    } else if (newCount < old) {
        queue_cv_.notify_all();
    }
}

void ThreadManager::enable_auto_adjust(bool enable) {
    auto_adjust_enabled_ = enable;
}

void ThreadManager::worker_loop(size_t index) {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            queue_cv_.wait(lock, [this, index] {
                return stop_ || !task_queue_.empty() || index >= target_thread_count_.load();
            });

            if (index >= target_thread_count_.load() && task_queue_.empty()) {
                return;
            }
            if (stop_ && task_queue_.empty()) {
                return;
            }
            if (!task_queue_.empty()) {
                Task t = std::move(const_cast<Task&>(task_queue_.top()));
                task_queue_.pop();
                task = std::move(t.func);
            } else {
                continue;
            }
        }

        if (task) {
            task();
            active_worker_tasks_--;
        }
    }
}

void ThreadManager::monitor_loop() {
    while (true) {
        {
            std::unique_lock<std::mutex> lock(monitor_mutex_);
            if (monitor_cv_.wait_for(lock, ADJUST_INTERVAL,
                                      [this] { return monitor_stop_.load(); })) {
                break;
            }
        }

        if (monitor_stop_) break;
        if (!auto_adjust_enabled_) continue;

        size_t newTarget = calculate_target_thread_count();
        size_t current = target_thread_count_.load();
        if (newTarget != current) {
            ///< 直接调整线程数，但保持自动调节标志不变
            newTarget = std::clamp(newTarget, min_threads_, max_threads_);
            size_t old = target_thread_count_.exchange(newTarget);
            if (newTarget > old) {
                std::lock_guard<std::mutex> lock(queue_mutex_);
                for (size_t i = old; i < newTarget; ++i) {
                    workers_.emplace_back(&ThreadManager::worker_loop, this, i);
                }
            } else if (newTarget < old) {
                queue_cv_.notify_all();
            }
        }
    }
}

size_t ThreadManager::calculate_target_thread_count() {
    size_t queueSize = 0;
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        queueSize = task_queue_.size();
    }

    size_t current = target_thread_count_.load();

    if (queueSize > current * 2 && current < max_threads_) {
        size_t increment = std::max<size_t>(1, queueSize / current);
        return std::min(max_threads_, current + increment);
    } else if (queueSize == 0 && current > min_threads_) {
        return current - 1;
    }
    return current;
}

void ThreadManager::submit_to_main_thread(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(main_queue_mutex_);
        main_tasks_.push(std::move(task));
        active_main_tasks_++;
    }
}

void ThreadManager::process_main_thread_tasks() {
    std::queue<std::function<void()>> localQueue;
    {
        std::lock_guard<std::mutex> lock(main_queue_mutex_);
        localQueue.swap(main_tasks_);
    }
    while (!localQueue.empty()) {
        localQueue.front()();
        localQueue.pop();
        active_main_tasks_--;
    }
}

void ThreadManager::wait_for_all_tasks() {
    while (active_worker_tasks_.load() > 0 || active_main_tasks_.load() > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

size_t ThreadManager::active_task_count() const {
    return active_worker_tasks_.load() + active_main_tasks_.load();
}