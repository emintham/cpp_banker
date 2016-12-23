banker:
	clang++ -std=c++14 -O3 -Wall -g banker.cpp board.cpp emm.cpp -o banker

rollout:
	clang++ -std=c++14 -O3 -Wall -g rollout.cpp board.cpp emm.cpp -o rollout

tests:
	clang++ -std=c++14 -Wall test_main.cpp test_board.cpp board.cpp -o test
