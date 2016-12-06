#ifndef __EMM_H__
#define __EMM_H__

#include "board.h"

class EMM {
  static bool markReuse;
  static int reusedValues;

public:
  static void solveBestMove();
  static void getMaxScore();

private:
  static int heuristicScore(Board *b);
  static float bestMove(Board *b, int nextTile, int depth, int* source, int* dest);
  static float expectiminimax(Board* board, int depth);
};

#endif
