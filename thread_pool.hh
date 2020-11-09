#ifndef MR_THREAD_POOL_
#define MR_THREAD_POOL_

#include <mutex>
#include <queue>
#include <tuple>
#include <vector>
#include <stdbool.h>
#include <future>

template <typename T, typename W>
class ThreadPool {

    public:
        std::queue<T> work_queue;
        std::vector<std::future<void>> working_future_vector;

        W worker;
        
        std::mutex work_count_mutex;
        std::condition_variable work_fill;

        int thread_num;

        unsigned long long counter = 0;
        bool terminate = false;

        ThreadPool(int thread_num, std::queue<T> work_queue, W worker) {
            this->thread_num = thread_num;
            this->worker = worker;
            this->work_queue = work_queue;
        }

        std::future<void> lauch_thread() {
            return async(this->thread_helper, this);
        }

        static void thread_helper(ThreadPool* pool) {
            T job_input;
            // printf(" thread\n");
            while (true) {
                // printf("new round\n");
                {
                    std::unique_lock<std::mutex> lock (pool->work_count_mutex);
                    while (pool->work_queue.empty()) {
                        // printf("quueue empty\n");
                        if (pool->terminate) {
                            // printf("terminate\n");
                            return ;
                        }
                        pool->work_fill.wait(lock);
                    }

                    // printf("got job\n");
                    job_input = pool->work_queue.front();
                    pool->work_queue.pop();
                    // printf("%lu\n", pool->work_queue.size());

                    pool->counter++;
                }

                // printf("execute job\n");
                std::apply(pool->worker, job_input);
                // printf("executed job, go to the next round\n");
            }
        }

        void add_job(T input) {
            {
                std::unique_lock<std::mutex> lock (this->work_count_mutex);
                this->work_queue.push(input);
            }
            this->work_fill.notify_one();
        }

        void start_jobs() {
            for (int i = 0; i < this->thread_num; i++) {
                working_future_vector.push_back(this->lauch_thread());
            }
        }

        static void job_helper(T job_input, W worker) {
            std::apply(worker, job_input);
        }

        void terminate_and_wait() {
            this->terminate = true;
            this->work_fill.notify_all();
            for (const std::future<void> & element : this->working_future_vector) {
                element.wait();
            }
        }
        
};

#endif 
