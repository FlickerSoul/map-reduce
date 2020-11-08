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
                T job_input = this->work_queue.front();
                this->work_queue.pop();

                // printf("popped job\n");
                // printf("try lock\n");

                std::unique_lock<std::mutex> lock (this->work_count_mutex);
                // printf("locked\n");
                while (this->thread_num == this->current_running_thread_num) {
                    printf("wait lock\n");
                    this->accepting_work.wait(lock);
                }

                // worker thread 
                this->working_future_vector.push_back(std::async(this->job_helper, job_input, this));

                this->counter++;

                // printf("add job\n");
                // printf("released lock\n");
            }

            // printf("wait job to finish\n");
            for (auto const & element: this->working_future_vector) {
                element.wait();
            }
            // printf("finished all jobs\n");
        }

        void remove_worked_thread() {
            // printf("getting cleaning lock\n");
            std::unique_lock<std::mutex> lock (this->work_count_mutex);
            // printf("got cleaning lock\n");
            // printf("decrease counter");
            this->current_running_thread_num -= 1;
            // printf("unlock cleaning lock\n");
            this->accepting_work.notify_one();
            // printf("nofity to put jobs\n");
        };

        static void job_helper(T job_input, ThreadPool* pool) {
            std::apply(pool->worker, job_input);
            pool->remove_worked_thread();
        }
        
};
