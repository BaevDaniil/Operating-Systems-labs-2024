#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    ThreadPool(size_t num_threads) {
        for (size_t i = 0; i < num_threads; ++i) {
            threads.emplace_back(&ThreadPool::worker_thread, this);
        }
    }

    template <typename F>
    void enqueue(F&& f) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            tasks.push(std::forward<F>(f));
        }
        cv.notify_one();
    }

    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            done = true;
        }
        cv.notify_all();
        for (std::thread& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

private:

    void worker_thread() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                cv.wait(lock, [this]() { return !tasks.empty() || done; });

                if (done && tasks.empty()) {
                    return;
                }

                task = std::move(tasks.front());
                tasks.pop();
            }
            task();
        }
    }

    std::vector<std::thread> threads;           
    std::queue<std::function<void()>> tasks;       
    std::mutex queue_mutex;                        
    std::condition_variable cv;                    
    bool done = false;                             
};
