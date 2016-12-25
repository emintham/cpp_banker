.PHONY: clean

CC = clang++
CFLAGS = -std=c++14 -O3 -Wall -pedantic -g
BENCHMARK_INCLUDE = -lbenchmark

SRCS = board.cpp emm.cpp
TEST_SRCS = test_board.cpp
TARGETS = banker rollout test performanceTest benchmarks

banker: banker.cpp $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $< -o $@

rollout: rollout.cpp $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $< -o $@

test: test_main.cpp board.cpp $(TEST_SRCS)
	$(CC) $(CFLAGS) $^ -o $@

performanceTest: performanceTest.cpp $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $< -o $@

benchmarks: benchmarks.cpp board.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(BENCHMARK_INCLUDE)

clean:
	$(RM) $(TARGETS) callgrind.out.*
