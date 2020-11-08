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
        std::condition_variable accepting_work;
        std::condition_variable work_fill;

        int thread_num;
        int current_running_thread_num;

        int counter = 0;
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
            while (true) {
                // printf("new round\n");
                {
                    std::unique_lock<std::mutex> lock (pool->work_count_mutex);
                    while (pool->work_queue.empty()) {
                        if (pool->terminate) {
                            return ;
                        }
                        pool->work_fill.wait(lock);
                    }

                    // printf("get job\n");
                    job_input = pool->work_queue.front();
                    pool->work_queue.pop();
                    // printf("%lu\n", pool->work_queue.size());

                    pool->counter++;
                }

                // printf("execute job\n");
                job_helper(job_input, pool->worker);
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
            // mapper thread 

            for (int i = 0; i < this->thread_num; i++) {
                working_future_vector.push_back(this->lauch_thread());
            }

            // while (!this->work_queue.empty()) {
            //     T job_input = this->work_queue.front();
            //     this->work_queue.pop();

            //     // printf("popped job\n");
            //     // printf("try lock\n");

            //     std::unique_lock<std::mutex> lock (this->work_count_mutex);
            //     // printf("locked\n");
            //     while (this->thread_num == this->current_running_thread_num) {
            //         printf("wait lock\n");
            //         this->accepting_work.wait(lock);
            //     }

            //     // worker thread 
            //     this->working_future_vector.push_back(std::async(this->job_helper, job_input, this));

            //     this->counter++;

            //     printf("add job %i\n", this->counter);
            //     // printf("released lock\n");
            // }

            // printf("wait job to finish\n");
            // for (auto const & element: this->working_future_vector) {
            //     element.wait();
            // }
            // printf("finished all jobs\n");
        }

        // void remove_worked_thread() {
        //     // printf("getting cleaning lock\n");
        //     std::unique_lock<std::mutex> lock (this->work_count_mutex);
        //     // printf("got cleaning lock\n");
        //     // printf("decrease counter");
        //     this->current_running_thread_num -= 1;
        //     // printf("unlock cleaning lock\n");
        //     this->accepting_work.notify_one();
        //     // printf("nofity to put jobs\n");
        // };

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
