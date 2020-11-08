#include <mutex>
#include <queue>
#include <stdbool.h>
#include <map>
#include <future>

template <typename T, typename W>
class ThreadPool {

    public:
        ThreadPool(int thread_num, W worker, std::queue<std::tuple<T>> work_queue);

        void add_job(std::tuple<T> input);
        void run_jobs();

    private: 
        std::queue<std::tuple<T>> work_queue;
        std::map<std::tuple<T>, std::future<void>> working_future_map;

        W worker;
        
        std::mutex work_count_lock;
        std::condition_variable accepting_work;

        int thread_num;
        int current_running_thread_num;

        int counter = 0;
        bool terminate = false;

        void remove_worked_thread(std::tuple<T> job_input);
        void job_helper(std::tuple<T> job_input);
        
};
