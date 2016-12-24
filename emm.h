#ifndef __EMM_H__
#define __EMM_H__

#include "board.h"

class EMM {
  public:
    static unsigned long leafNodesExplored;

    static void rollout();
    static int rolloutOnce(int depth);
    static void commandParser();
    static BoardPtr solveBestMove(const BoardPtr& b, const Tile& nextTile, int depth, int *dist);
    static BoardPtr solveBestMove(const BoardPtr& b, const Tile& nextTile, int depth, int *dist, bool verbose);
    static BoardPtr handleLawsuit(std::istringstream& currentLine, std::ofstream& tileFile, const BoardPtr& b, const int depth);
    static BoardPtr handleBonus(std::istringstream& currentLine, std::ofstream& tileFile, const BoardPtr& b, const int depth);
    static BoardPtr handleNonProfit(std::istringstream& currentLine, std::ofstream& tileFile, const BoardPtr& b, const int depth);
    static void handleDebug(std::istringstream& currentLine, std::ofstream& tileFile, const BoardPtr& b);
    static BoardPtr handleTile(const int nextTile, std::ofstream& tileFile, const BoardPtr& b, const int depth);

  private:
    static bool markReuse;
    static int reusedValues;

    static int heuristicScore(const BoardPtr& b);
    static float bestMove(const BoardPtr& b, const Tile& nextTile, int depth, int* source, int* dest);
    static float expectiminimax(const BoardPtr& board, int depth);
};

#endif
