.PHONY: clean

CC = clang++
CFLAGS = -std=c++14 -O3 -Wall -pedantic -g
BENCHMARK_INCLUDE = -lbenchmark

SRCS = board.cpp emm.cpp
TARGETS = banker rollout test performanceTest benchmarks

banker: $(SRCS) banker.cpp
	$(CC) $(CFLAGS) $(SRCS) banker.cpp -o $@

rollout: $(SRCS) rollout.cpp
	$(CC) $(CFLAGS) $(SRCS) rollout.cpp -o $@

test: board.cpp test_main.cpp test_board.cpp
	$(CC) $(CFLAGS) board.cpp test_board.cpp test_main.cpp -o $@

performanceTest: $(SRCS) performanceTest.cpp
	$(CC) $(CFLAGS) $(SRCS) performanceTest.cpp -o $@

benchmarks: board.cpp
	$(CC) $(CFLAGS) board.cpp benchmarks.cpp -o $@ $(BENCHMARK_INCLUDE)

clean:
	$(RM) $(TARGETS) callgrind.out.*
