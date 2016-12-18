#ifndef __BOARD_H__
#define __BOARD_H__

#include <array>
#include <bitset>
#include <tuple>
#include <vector>

#include "constants.h"

class Board {

public:
  std::array<int, BOARD_SIZE> board = {0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0,
                                       0, 0, 1, 0, 0,
                                       0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0};
  std::bitset<BOARD_SIZE> competitors;
  int competitorTimers[BOARD_SIZE] = {0};
  int moves = 0;
  int score = 10;
  int cash = 10;

  // ----- Methods ----------
  // Util methods
  void print();
  bool isEmpty(int position);
  bool isCompetitor(int position);
  bool isBankrupt();
  float competitorCosts();
  std::vector<std::tuple<int, int, int>> getMoveset();

  static void printMove(int source, int dest);
  static int getRandomTile(int score);

  // Board modifying methods
  void addCompetitor(int pos, int value);
  void clearCompetitor(int pos);
  Board* move(int source, int dest, int nextTile);

private:
  void updateTimer();
};

#endif
