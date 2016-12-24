#ifndef __BOARD_H__
#define __BOARD_H__

#include <array>
#include <memory>
#include <ostream>
#include <tuple>
#include <vector>

#include "constants.h"
#include "tile.h"

class Board;

typedef std::shared_ptr<Board> BoardPtr;

class Board {
  public:
    std::array<Tile, BOARD_SIZE> board = {Tile(0), Tile(0), Tile(0), Tile(0), Tile(0),
                                          Tile(0), Tile(0), Tile(0), Tile(0), Tile(0),
                                          Tile(0), Tile(0), Tile(1), Tile(0), Tile(0),
                                          Tile(0), Tile(0), Tile(0), Tile(0), Tile(0),
                                          Tile(0), Tile(0), Tile(0), Tile(0), Tile(0)};
    int competitorTimers[BOARD_SIZE] = {0};
    int bonus[BOARD_SIZE] = {0};
    int score = 10;
    int cash = 10;

    // ----- Methods ----------
    // Util methods
    void printCompetitorTimers() const;
    bool isEmpty(int position) const;
    bool isLawsuit(int position) const;
    bool isPosLawsuit(int position) const;
    bool isNegLawsuit(int position) const;
    bool isNonProfit(int position) const;
    bool isCompetitor(int position) const;
    int numCompetitors() const;
    bool isBankrupt() const;
    int competitorCosts() const;
    std::vector<std::tuple<int, int, int>> getMoveset() const;

    static void printMove(const int source, const int dest);
    static const Tile getRandomTile(int score);

    // Board modifying methods
    void addCompetitor(int pos, Tile tile);
    void clearCompetitor(int pos);
    void addBonus(int pos, int value);
    BoardPtr move(const int source, const int dest, const Tile& nextTile);

    friend std::ostream& operator<<(std::ostream& os, const Board b);

  private:
    int competitors = 0;

    void updateTimer();
    void updateBonus();
    BoardPtr walk(const int source, const int dest, const Tile& nextTile) const;
    BoardPtr jump(const int source, const int dest, const Tile& nextTile, const int start, const int dist, const bool horizontalJump) const;
};

#endif
