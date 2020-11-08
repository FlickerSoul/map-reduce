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

test_wish_utils: test_wish_utils.o
	./test_wish_utils.o < wish_utils_test.in
	rm a.txt

wish: wish.c wish_utils.o commands.o parallel_commands.o
	$(CXX) $(CXX_FLAGS) -o $@ $^

leak_test: test_commands.o test_parallel_commands.o test_wish_utils.o
	$(VALGRIND) ./test_commands.o 
	$(VALGRIND) ./test_parallel_commands.o 
	$(VALGRIND) ./test_wish_utils.o < wish_utils_test.in
	rm a.txt

test_wish: wish
	./test-wish.sh

run: wish
	./wish

vrun: wish
	$(VALGRIND) ./wish

clean:
	rm -rf *~ *.o $(TARGETS) *.dSYM