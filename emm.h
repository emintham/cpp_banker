#ifndef __EMM_H__
#define __EMM_H__

#include "board.h"

class EMM {
  public:
    bool markReuse = false;
    bool countLeafNodes = false;
    int reusedValues = 0;
    unsigned long leafNodesExplored = 0;

    void rollout(int depth);
    int rolloutOnce(int depth);
    void commandParser(int depth);
    BoardPtr solveBestMove(const BoardPtr& b, const Tile& nextTile, int depth, int *dist);
    BoardPtr solveBestMove(const BoardPtr& b, const Tile& nextTile, int depth, int *dist, bool verbose);
    BoardPtr handleLawsuit(std::istringstream& currentLine, std::ofstream& tileFile, const BoardPtr& b, const int depth);
    BoardPtr handleBonus(std::istringstream& currentLine, std::ofstream& tileFile, const BoardPtr& b, const int depth);
    BoardPtr handleNonProfit(std::istringstream& currentLine, std::ofstream& tileFile, const BoardPtr& b, const int depth);
    void handleDebug(std::istringstream& currentLine, std::ofstream& tileFile, const BoardPtr& b);
    BoardPtr handleTile(const int nextTile, std::ofstream& tileFile, const BoardPtr& b, const int depth);

  private:
    int heuristicScore(const BoardPtr& b);
    float bestMove(const BoardPtr& b, const Tile& nextTile, int depth, int* source, int* dest);
    float expectiminimax(const BoardPtr& board, int depth);
};

#endif
