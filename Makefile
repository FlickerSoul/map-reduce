CXX=g++
CXX_FLAGS=-g -std=c++17
VALGRIND=valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes
.PHONY: all clean

all: $(TARGETS)


thread_pool.o: thread_pool.cc thread_pool.hh
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

utilities.o: utilities.cc utilities.hh
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

map_reduce.o: map_reduce.cc map_reduce.hh
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

word_count.o: ./tests/word_count.cc thread_pool.o map_reduce.o utilities.o
	$(CXX) $(CXX_FLAGS) -o $@ $^	

grep.o: ./tests/grep.cc thread_pool.o map_reduce.o utilities.o
	$(CXX) $(CXX_FLAGS) -o $@ $^

word_count: clean word_count.o ./tests/word_count_tests/
	./word_count.o ./tests/word_count_tests/1.in ./tests/word_count_tests/2.in ./tests/word_count_tests/3.in ./tests/word_count_tests/4.in ./tests/word_count_tests/5.in

grep: clean grep.o ./tests/grep_tests/
	./grep.o Dictum ./tests/grep_tests/1.in ./tests/grep_tests/2.in
	./grep.o Dictum ./tests/grep_tests/2.in
	./grep.o dictum ./tests/grep_tests/1.in ./tests/grep_tests/2.in
	./grep.o abc ./tests/grep_tests/1.in


thread_pool_test.o: thread_pool_test.cc thread_pool.o
	$(CXX) $(CXX_FLAGS) -o $@ $^

utilities_test.o: utilities_test.cc utilities.o
	$(CXX) $(CXX_FLAGS) -o $@ $^

map_reduce_test.o: map_reduce_test.cc map_reduce.o utilities.o
	$(CXX) $(CXX_FLAGS) -o $@ $^

test_thread_pool: thread_pool_test.o
	./thread_pool_test.o

utilities_test: utilities_test.o
	./utilities_test.o

map_reduce_test: map_reduce_test.o
	./map_reduce_test.o

clean:
	rm -rf *~ *.o $(TARGETS) *.dSYM