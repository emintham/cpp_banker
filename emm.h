#ifndef __EMM_H__
#define __EMM_H__

#include "board.h"

class EMM {
  static bool markReuse;
  static int reusedValues;

public:
  static void rollout();
  static int rolloutOnce(int depth);
  static void commandParser();
  static Board* solveBestMove(Board *b, const Tile& nextTile, int depth, int *dist);
  static Board* solveBestMove(Board *b, const Tile& nextTile, int depth, int *dist, bool verbose);
  static Board* handleLawsuit(std::istringstream& currentLine, std::ofstream& tileFile, Board* b, const int depth);
  static Board* handleBonus(std::istringstream& currentLine, std::ofstream& tileFile, Board* b, const int depth);
  static Board* handleNonProfit(std::istringstream& currentLine, std::ofstream& tileFile, Board* b, const int depth);
  static void handleDebug(std::istringstream& currentLine, std::ofstream& tileFile, Board* b);
  static Board* handleMyTile(const int nextTile, std::ofstream& tileFile, Board* b, const int depth);

private:
  static int heuristicScore(Board *b);
  static float bestMove(Board *b, const Tile& nextTile, int depth, int* source, int* dest);
  static float expectiminimax(Board* board, int depth);
};

#endif
