bin:
	clang++ -std=c++14 -O3 -Wall banker.cpp board.cpp emm.cpp -o banker

tests:
	clang++ -std=c++14 -Wall test_main.cpp test_board.cpp board.cpp -o test
