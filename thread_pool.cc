#include "thread_pool.hh"

template <typename T, typename W>
ThreadPool<T, W>::ThreadPool(int thread_num, W worker, std::queue<std::tuple<T>> work_queue) {
    this->thread_num = thread_num;
    this->worker = worker;
    this->work_queue = work_queue;
}

template <typename T, typename W>
void ThreadPool<T, W>::remove_worked_thread(std::tuple<T> work_input) {
    this->work_count_lock.lock();
    this->working_future_map.erase(work_input);
    this->current_running_thread_num -= 1;
    this->work_count_lock.unlock();
    this->accepting_work.notify_one();
}


template <typename T, typename W>
void ThreadPool<T, W>::run_jobs() {
    // mapper thread 
    while (!this->work_queue.empty()) {
        std::tuple<T> job_input = this->work_queue.pop();

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

template <typename T, typename W>
void ThreadPool<T, W>::job_helper(std::tuple<T> job_input) {
    std::apply(this->worker, job_input);
    this->remove_worked_thread(job_input);
}