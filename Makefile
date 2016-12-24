.PHONY: clean

CC = clang++
CFLAGS = -std=c++14 -O3 -Wall -g

SRCS = board.cpp emm.cpp

banker: $(SRCS) banker.cpp
	$(CC) $(CFLAGS) $(SRCS) banker.cpp -o $@

rollout: $(SRCS) rollout.cpp
	$(CC) $(CFLAGS) $(SRCS) rollout.cpp -o $@

test: board.cpp test_main.cpp test_board.cpp
	$(CC) $(CFLAGS) board.cpp test_board.cpp test_main.cpp -o $@

clean:
	$(RM) banker rollout test callgrind.out.*
