#include <algorithm>
#include <iterator>
#include <tuple>

#include "catch.hpp"

#include "constants.h"
#include "board.h"


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

  REQUIRE(b->competitors.count() == 0);

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

  b->competitors.set(0);
  REQUIRE(!b->isEmpty(0));

  delete b;
}

TEST_CASE("isCompetitor", "[Board]") {
  Board *b = new Board();

  for (int i=0; i<BOARD_SIZE; i++) {
    REQUIRE(!b->isCompetitor(i));
  }

  b->competitors.set(0);
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
    b->competitors.set(i);
  }

  REQUIRE(b->competitorCosts() == 0);

  // Make second row -1 to -5-- these should be included in the costs
  int j = -1;
  for (int i=5; i<10; i++, j--) {
    b->competitors.set(i);
    b->board[i] = j;
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

  b->addCompetitor(0, -1);
  REQUIRE(b->isCompetitor(0));
  REQUIRE(b->competitorCosts() == -1);

  b->addCompetitor(5, -2);
  REQUIRE(b->isCompetitor(5));
  REQUIRE(b->competitorCosts() == -3);

  delete b;
}

TEST_CASE("clearCompetitor", "[Board]") {
  Board *b = new Board();

  b->addCompetitor(0, -1);
  b->addCompetitor(5, -2);

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

  auto b2 = b->move(12, 13, 1, false);
  REQUIRE(b2->bonus[0] == 4);

  b2->addBonus(1, 10);
  auto b3 = b2->move(12, 11, 2, false);
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

  auto b2 = b->move(12, 11, 1, false);
  REQUIRE(b2->cash == b->cash + bonusVal);

  delete b;
  delete b2;
}

TEST_CASE("addLawsuit", "[Board]") {
  Board *b = new Board();

  b->addLawsuit(11, false);
  REQUIRE(b->positiveLawsuits[11]);
  REQUIRE(!b->negativeLawsuits[11]);

  b->addLawsuit(10, true);
  REQUIRE(b->negativeLawsuits[10]);
  REQUIRE(!b->positiveLawsuits[10]);

  delete b;
}

TEST_CASE("Lawsuit effects", "[Board]") {
  Board *b = new Board();

  b->addLawsuit(11, false);
  REQUIRE(b->positiveLawsuits[11]);

  auto b2 = b->move(12, 11, 1, false);
  REQUIRE(b2->board[11] == 2);
  REQUIRE(b2->board[12] == 1);
  REQUIRE(!b2->positiveLawsuits[11]);

  b2->addLawsuit(13, true);
  REQUIRE(b2->negativeLawsuits[13]);

  auto b3 = b2->move(12, 13, 1, false);
  REQUIRE(b3->board[11] == 2);
  REQUIRE(b3->board[12] == 1);
  REQUIRE(b3->board[13] == 0);
  REQUIRE(!b3->negativeLawsuits[13]);

  delete b;
  delete b2;
  delete b3;
}

TEST_CASE("nonProfit is not a valid dest", "[Board]") {
  Board *b = new Board();

  REQUIRE(b->getMoveset().size() == 4);

  // Non Profit at a further range is not a destination
  b->addNonProfit(14, 1);
  REQUIRE(b->getMoveset().size() == 4);

  // Non Profit next to tile blocks it
  b->addNonProfit(11, 1);
  REQUIRE(b->getMoveset().size() == 3);

  delete b;
}
