#include <mutex>
#include <queue>
#include <tuple>
#include <stdbool.h>
#include <map>
#include <future>

template <typename T, typename W>
class ThreadPool {

    public:
        std::queue<T> work_queue;
        std::map<T, std::future<void>> working_future_map;

        W worker;
        
        std::mutex work_count_lock;
        std::condition_variable accepting_work;

        int thread_num;
        int current_running_thread_num;

        int counter = 0;
        bool terminate = false;
        
        ThreadPool(int thread_num, std::queue<T> work_queue, W worker) {
            this->thread_num = thread_num;
            this->worker = worker;
            this->work_queue = work_queue;
        }

        // void add_job(T input);

        void run_jobs() {
            // mapper thread 
            while (!this->work_queue.empty()) {
                T job_input = this->work_queue.pop();

                this->work_count_lock.lock();
                while (this->thread_num == this->current_running_thread_num) {
                    this->accepting_work.wait(this->work_count_lock);
                }

                // worker thread 
                this->working_future_map.emplace(job_input, 
                    std::async(this->job_helper, 
                            job_input)
                );

                this->work_count_lock.unlock();
            }

            for (auto const &[key, value]: this->working_future_map) {
                value.wait();
            }
        }

        void remove_worked_thread(T job_input) {
            this->work_count_lock.lock();
            this->working_future_map.erase(work_input);
            this->current_running_thread_num -= 1;
            this->work_count_lock.unlock();
            this->accepting_work.notify_one();
        };

        void job_helper(T job_input) {
            std::apply(this->worker, job_input);
            this->remove_worked_thread(job_input);
        }
        
};
