#include <algorithm>
#include <iterator>
#include <tuple>
#include <iostream>

#include "catch.hpp"

#include "constants.h"
#include "board.h"
#include "tile.h"


TEST_CASE("Board initialization", "[Board]") {
  Board *b = new Board();

  REQUIRE(b->score == 10);
  REQUIRE(b->cash == 10);

  for (int i=0; i<BOARD_SIZE; i++) {
    if (i == 12) {
      REQUIRE(b->board[i] == 1);
    } else {
      REQUIRE(b->board[i] == 0);
    }
  }

  for (int i=0; i<BOARD_SIZE; i++) {
    REQUIRE(b->competitorTimers[i] == 0);
    REQUIRE(b->bonus[i] == 0);
  }

  REQUIRE(b->numCompetitors() == 0);

  delete b;
}

TEST_CASE("isEmpty", "[Board]") {
  Board *b = new Board();

  for (int i=0; i<BOARD_SIZE; i++) {
    if (i == 12) {
      REQUIRE(!b->isEmpty(i));
    } else {
      REQUIRE(b->isEmpty(i));
    }
  }

  b->board[0] = Tile(0, competitor);
  REQUIRE(!b->isEmpty(0));

  delete b;
}

TEST_CASE("isCompetitor", "[Board]") {
  Board *b = new Board();

  for (int i=0; i<BOARD_SIZE; i++) {
    REQUIRE(!b->isCompetitor(i));
  }

  b->board[0] = Tile(0, competitor);
  REQUIRE(b->isCompetitor(0));

  delete b;
}

TEST_CASE("isBankrupt", "[Board]") {
  Board *b = new Board();

  REQUIRE(!b->isBankrupt());

  b->cash = 0;
  REQUIRE(!b->isBankrupt());

  b->cash = -1;
  REQUIRE(b->isBankrupt());

  delete b;
}

TEST_CASE("competitorCosts", "[Board]") {
  Board *b = new Board();

  REQUIRE(b->competitorCosts() == 0);

  // Make first row all 0-competitors-- these should not be included in the
  // costs
  for (int i=0; i<5; i++) {
    b->board[i] = Tile(0, competitor);
  }

  REQUIRE(b->competitorCosts() == 0);

  // Make second row -1 to -5-- these should be included in the costs
  int j = -1;
  for (int i=5; i<10; i++, j--) {
    b->board[i] = Tile(j, competitor);
  }

  REQUIRE(b->competitorCosts() == -15);

  delete b;
}

/*
 * REQUIRE_MOVESET_EQUAL:
 *    Test that a given board has moveset equal to the expected move set.
 */
void REQUIRE_MOVESET_EQUAL(
        Board *b,
        std::vector<std::tuple<int, int, int>> expectedMoveset) {

  auto moveset = b->getMoveset();

  REQUIRE(moveset.size() == expectedMoveset.size());

  for (auto &move: expectedMoveset) {
    auto locate = std::find(moveset.begin(), moveset.end(), move);

    REQUIRE(locate != moveset.end());
  }

}

TEST_CASE("getMoveset", "[Board]") {
  Board *b = new Board();

  // --------------------------------------------------------------------------
  // Case A: 1 tile, 4 walks
  std::vector<std::tuple<int, int, int>> expectedMovesetA {
    // Source, Dest, Dist
    {12,  7, 1},
    {12, 11, 1},
    {12, 13, 1},
    {12, 17, 1}
  };

  REQUIRE_MOVESET_EQUAL(b, expectedMovesetA);

  // --------------------------------------------------------------------------
  // Case B: 2 same tiles, 7 walks 2 jumps
  b->board[12] = 0;
  b->board[11] = 1;
  b->board[14] = 1;

  std::vector<std::tuple<int, int, int>> expectedMovesetB {
    // Source, Dest, Dist
    {11,  6, 1},
    {11, 10, 1},
    {11, 12, 1},
    {11, 16, 1},
    {14,  9, 1},
    {14, 13, 1},
    {14, 19, 1},
    {14, 11, 3},
    {11, 14, 3}
  };

  REQUIRE_MOVESET_EQUAL(b, expectedMovesetB);

  delete b;
}

TEST_CASE("addCompetitor", "[Board]") {
  Board *b = new Board();

  REQUIRE(b->competitorCosts() == 0);

  b->addCompetitor(0, Tile(1, competitor));
  REQUIRE(b->isCompetitor(0));
  REQUIRE(b->competitorCosts() == 1);

  b->addCompetitor(5, Tile(2, competitor));
  REQUIRE(b->isCompetitor(5));
  REQUIRE(b->competitorCosts() == 3);

  delete b;
}

TEST_CASE("clearCompetitor", "[Board]") {
  Board *b = new Board();

  b->addCompetitor(0, Tile(1, competitor));
  b->addCompetitor(5, Tile(2, competitor));

  REQUIRE(b->isCompetitor(0));
  REQUIRE(b->isCompetitor(5));

  b->clearCompetitor(0);
  REQUIRE(!b->isCompetitor(0));
  REQUIRE(b->isCompetitor(5));


  b->clearCompetitor(5);
  REQUIRE(!b->isCompetitor(0));
  REQUIRE(!b->isCompetitor(5));

  delete b;
}

TEST_CASE("addBonus", "[Board]") {
  Board *b = new Board();

  b->addBonus(0, 5);
  REQUIRE(b->bonus[0] == 5);

  delete b;
}

TEST_CASE("bonus is decreased after each move", "[Board]") {
  Board *b = new Board();

  b->addBonus(0, 5);
  REQUIRE(b->bonus[0] == 5);

  auto b2 = b->move(12, 13, Tile(1));
  REQUIRE(b2->bonus[0] == 4);

  b2->addBonus(1, 10);
  auto b3 = b2->move(12, 11, Tile(2));
  REQUIRE(b3->bonus[0] == 3);
  REQUIRE(b3->bonus[1] == 9);

  delete b;
  delete b2;
  delete b3;
}

TEST_CASE("bonus is added to cash", "[Board]") {
  Board *b = new Board();
  const int bonusVal = 5;

  b->addBonus(11, bonusVal);
  REQUIRE(b->bonus[11] == bonusVal);

  auto b2 = b->move(12, 11, Tile(1));
  REQUIRE(b2->cash == b->cash + bonusVal);

  delete b;
  delete b2;
}

TEST_CASE("Lawsuits", "[Board]") {
  Board *b = new Board();

  b->board[11] = Tile(0, positiveLawsuit);
  REQUIRE(b->isPosLawsuit(11));
  REQUIRE(!b->isNegLawsuit(11));

  b->board[10] = Tile(0, negativeLawsuit);
  REQUIRE(!b->isPosLawsuit(10));
  REQUIRE(b->isNegLawsuit(10));

  auto b2 = b->move(12, 11, Tile(1));
  REQUIRE(b2->board[11] == 2);
  REQUIRE(b2->board[12] == 1);
  REQUIRE(!b2->isPosLawsuit(11));

  b2->board[13] = Tile(0, negativeLawsuit);
  REQUIRE(b2->isNegLawsuit(13));

  auto b3 = b2->move(12, 13, Tile(1));
  REQUIRE(b3->board[11] == 2);
  REQUIRE(b3->board[12] == 1);
  REQUIRE(b3->board[13] == 0);
  REQUIRE(!b3->isNegLawsuit(13));

  delete b;
  delete b2;
  delete b3;
}

TEST_CASE("nonProfit is not a valid dest", "[Board]") {
  Board *b = new Board();

  REQUIRE(b->getMoveset().size() == 4);

  // Non Profit at a further range is not a destination
  b->board[14] = Tile(1, nonProfit);
  REQUIRE(b->getMoveset().size() == 4);

  // Non Profit next to tile blocks it
  b->board[11] = Tile(1, nonProfit);
  REQUIRE(b->getMoveset().size() == 3);

  delete b;
}

TEST_CASE("jump", "[Board]") {
  Board *b = new Board();

  b->board[10] = Tile(1);

  // 4 walks for 11, 3 walks for 10, 2 jumps between them
  REQUIRE(b->getMoveset().size() == 9);

  auto b2 = b->move(12, 10, Tile(1));
  REQUIRE(b2->board[10] == Tile(2));
  REQUIRE(b2->board[12] == Tile());
  REQUIRE(b2->cash == b->cash + 2);
  REQUIRE(b2->score == b->score + 2);

  b2->board[12] = Tile(1);

  // no jumps between two different tiles
  REQUIRE(b2->getMoveset().size() == 7);

  // --------------------------------------------------------------------------
  // Jump Fusion destroys nonProfit and competitors
  b2->board[13] = Tile(1, nonProfit);
  b2->board[11] = Tile(1, competitor);
  b2->board[14] = Tile(2);

  // 2 walks each for 3 tiles, 2 jumps between two tiles
  REQUIRE(b2->getMoveset().size() == 8);

  auto b3 = b2->move(10, 14, Tile(4));
  REQUIRE(b3->board[10] == Tile());
  REQUIRE(b3->board[11] == Tile());
  REQUIRE(b3->board[12] == Tile(1));
  REQUIRE(b3->board[13] == Tile());
  REQUIRE(b3->board[14] == Tile(3));

  const int fusionBonus = 3;
  const int comboBonus = 4;
  REQUIRE(b3->cash == b2->cash + fusionBonus + comboBonus);
  REQUIRE(b3->score == b2->score + fusionBonus + comboBonus);

  delete b;
  delete b2;
  delete b3;
}

TEST_CASE("walk", "[Board]") {
  Board *b = new Board();

  b->board[11] = Tile(1);

  // Both tiles can walk all directions
  REQUIRE(b->getMoveset().size() == 8);

  auto b2 = b->move(11, 12, Tile(3));
  REQUIRE(b2->board[11] == Tile(3));
  REQUIRE(b2->board[12] == Tile(2));
  REQUIRE(b2->cash == b->cash + 2);
  REQUIRE(b2->score == b->score + 2);
  // 3 directions each (except in the direction of one another)
  REQUIRE(b2->getMoveset().size() == 6);

  // --------------------------------------------------------------------------
  // Test positive lawsuits is a valid source
  b2->board[10] = Tile(0, positiveLawsuit);

  // Lawsuit can move into adjacent tile but not into empty space
  REQUIRE(b2->getMoveset().size() == 7);

  auto b3 = b2->move(10, 11, Tile(1));
  REQUIRE(b3->board[10] == Tile());  // Lawsuit is gone
  REQUIRE(b3->board[11] == Tile(4)); // Tile increased by 1
  REQUIRE(b3->cash == b2->cash);     // Cash unchanged
  REQUIRE(b3->score == b2->score);   // Score unchanged

  // --------------------------------------------------------------------------
  // Test positive lawsuits is a valid destination
  b3->board[10] = Tile(0, positiveLawsuit);

  REQUIRE(b3->getMoveset().size() == 7);

  auto b4 = b3->move(11, 10, Tile(1));
  REQUIRE(b4->board[11] == Tile(1)); // Tile is replaced by new tile
  REQUIRE(b4->board[10] == Tile(5)); // Tile increased by 1
  REQUIRE(b4->cash == b3->cash);     // Cash unchanged
  REQUIRE(b4->score == b3->score);   // Score unchanged

  // --------------------------------------------------------------------------
  // Test negative lawsuits is a valid source
  b4->board[5] = Tile(0, negativeLawsuit);

  // Lawsuit can move into adjacent tile but not into empty space
  REQUIRE(b4->getMoveset().size() == 8);

  auto b5 = b4->move(5, 10, Tile(1));
  REQUIRE(b5->board[5] == Tile());   // Lawsuit is gone
  REQUIRE(b5->board[10] == Tile(4)); // Tile decreased by 1
  REQUIRE(b5->cash == b4->cash);     // Cash unchanged
  REQUIRE(b5->score == b4->score);   // Score unchanged

  // --------------------------------------------------------------------------
  // Test negative lawsuits is a valid destination
  b5->board[5] = Tile(0, negativeLawsuit);

  REQUIRE(b5->getMoveset().size() == 8);

  auto b6 = b5->move(10, 5, Tile(1));
  REQUIRE(b6->board[10] == Tile(1));  // Tile is replaced by new tile
  REQUIRE(b6->board[5] == Tile(3));  // Tile decreased by 1
  REQUIRE(b6->cash == b5->cash);     // Cash unchanged
  REQUIRE(b6->score == b5->score);   // Score unchanged

  delete b;
  delete b2;
  delete b3;
  delete b4;
  delete b5;
  delete b6;
}
