all:
	clang++ -std=c++14 -O3 -Wall banker.cpp board.cpp emm.cpp -o banker
