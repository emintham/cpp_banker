#ifndef __BOARD_H__
#define __BOARD_H__

#include <array>
#include <tuple>
#include <vector>

class Board {

public:
  std::array<int, 25> board = {0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0,
                               0, 0, 1, 0, 0,
                               0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0};
  bool competitors[25] = {false};
  int competitorTimers[25] = {0};
  int moves = 0;
  int score = 10;
  int cash = 10;

  // expectiminimax member variables
  static bool markReuse;
  static int reusedValues;

  // ----- Methods ----------
  // Util methods
  void print();
  bool isEmpty(int position);
  bool isCompetitor(int position);
  bool isBankrupt();
  float competitorCosts();
  std::vector<std::tuple<int, int>> getMoveset();

  // Board modifying methods
  void addCompetitor(int pos, int value);
  void clearCompetitor(int pos);
  Board* move(int source, int dest, int nextTile);

  // expectiminimax methods
  int heuristicScore();
  float bestMove(int nextTile, int depth, int* source, int* dest);
  static float EMM(Board* board, int depth);

private:
  void updateTimer();
};

#endif
