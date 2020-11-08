#include "thread_pool.hh"
#include <cassert>

void test_initialization() {
    int big_num = 100'000;
    std::function<int(int, int)> f = [&](int a, int b) {return a + b;};
    std::queue<std::tuple<int, int>> q;
    for (int i = 0; i < big_num; i++) {
        q.push(std::make_tuple(1, 1));
    }

    int thread_num = 2;

    ThreadPool<std::tuple<int, int>, decltype(f)> tp (thread_num, q, f);
    assert(tp.thread_num == thread_num);
    assert(tp.worker(1,2) == f(1,2));
    assert(tp.work_queue == q);

    tp.start_jobs();

    tp.terminate_and_wait();
    assert(tp.counter == big_num);
}

int main() {
    test_initialization();
}