#include <algorithm>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "constants.h"
#include "board.h"

std::ostream& operator<<(std::ostream& os, const Board b) {
  for (int i=0; i<BOARD_SIZE; i++) {
    if (i != 0 && i % 5 == 0) {
      os << '\n';
    }

    if (b.bonus[i]) {
      os << '$' << b.board[i].value;
    } else {
      os << b.board[i];
    }
  }

  os << '\n';

  return os;
}

void Board::printMove(const int source, const int dest) {
  for (int i=0; i<BOARD_SIZE; i++) {
    if (i != 0 && i % 5 == 0) {
      std::cout << '\n';
    }

    char c;
    if (i == source) {
      c = 'A';
    } else if (i == dest) {
      c = 'B';
    } else {
      c = '+';
    }

    std::ostringstream oss;
    oss << std::setw(2) << c;

    std::cout << oss.str() << ' ';
  }
  std::cout << '\n';
}

void Board::printCompetitorTimers() const {
  for (int i=0; i<BOARD_SIZE; i++) {
    if (i != 0 && i % 5 == 0) {
      std::cout << '\n';
    }

    std::ostringstream oss;
    oss << std::setw(2) << std::to_string(competitorTimers[i]);

    std::cout << oss.str() << ' ';
  }
  std::cout << '\n';
}

bool Board::isEmpty(int position) const {
  return board[position] == Tile();
}

bool Board::isCompetitor(int position) const {
  return board[position].tileType == competitor;
}

bool Board::isBankrupt() const {
  return cash < 0;
}

bool Board::isPosLawsuit(int pos) const {
  return board[pos].tileType == positiveLawsuit;
}

bool Board::isNegLawsuit(int pos) const {
  return board[pos].tileType == negativeLawsuit;
}

bool Board::isLawsuit(int pos) const {
  return this->isPosLawsuit(pos) || this->isNegLawsuit(pos);
}

bool Board::isNonProfit(int pos) const {
  return board[pos].tileType == nonProfit;
}

int Board::numCompetitors() const {
  return competitors;
}

int Board::competitorCosts() const {
  int total = 0;

  for (auto& tile: board) {
    total += tile.tileType == competitor ? tile.value : 0;
  }

  return total;
}

std::vector<std::tuple<int, int, int>> Board::getMoveset() const {
  std::vector<std::tuple<int, int, int>> allPossibleMoves;

  for (int src=0; src<BOARD_SIZE; src++) {
    if (this->isEmpty(src) || this->isNonProfit(src) || this->isCompetitor(src)) {
      continue;
    }

    for (const int& dest : possibleDest[src]) {
      const int diff = abs(src-dest);
      const int dist = diff/5 + diff%5;

      // If source is a lawsuit, it can only walk to an adjacent tile if it
      // isn't also a lawsuit
      if (this->isLawsuit(src)) {
        if (dist == 1 && !this->isEmpty(dest) && !this->isLawsuit(dest)) {
          allPossibleMoves.push_back(std::make_tuple(src, dest, dist));
        }

        continue;
      }

      bool walk = dist == 1 && (this->isEmpty(dest) || board[src] == board[dest] || this->isLawsuit(dest));
      bool jump = dist != 1 && board[src] == board[dest];

      if (!this->isNonProfit(dest) && (walk || jump)) {
        allPossibleMoves.push_back(std::make_tuple(src, dest, dist));
      }
    }
  }

  return allPossibleMoves;
}

void Board::addCompetitor(int pos, Tile tile) {
  board[pos] = tile;
  competitorTimers[pos] = 17;
  competitors++;
}

void Board::clearCompetitor(int pos) {
  board[pos] = Tile();
  competitorTimers[pos] = 0;
  competitors--;
}

void Board::addBonus(int pos, int value) {
  bonus[pos] = value;
}

void Board::updateTimer() {
  for (int i=0; i<BOARD_SIZE; i++) {
    if (!this->isCompetitor(i)) continue;

    if (!competitorTimers[i]) {
      board[i] = Tile(board[i].value + 1, competitor);
      competitorTimers[i] = 18;
      continue;
    }

    competitorTimers[i]--;
  }
}

void Board::updateBonus() {
  for (int i=0; i<BOARD_SIZE; i++) {
    if (bonus[i]) bonus[i]--;
  }
}

BoardPtr Board::walk(
        const int source,
        const int dest,
        const Tile& nextTile) const {
  const int sourceTile = this->board[source].value;
  const int destTile = this->board[dest].value;

   BoardPtr newBoard = std::make_shared<Board>(*this);

  // Moving lawsuit directly does not change cash or score
  if (this->isLawsuit(source)) {
    auto oldTile = newBoard->board[dest];

    if (this->isNegLawsuit(source)) {
      newBoard->board[dest] = Tile(oldTile.value - 1, oldTile.tileType);
    } else {
      newBoard->board[dest] = Tile(oldTile.value + 1, oldTile.tileType);
    }

    if (newBoard->isCompetitor(dest) && newBoard->board[dest].value < 0) {
      newBoard->clearCompetitor(dest);
    }

    newBoard->board[source] = Tile();

    return newBoard;
  }

  int newDest, cashDelta, scoreDelta;

  if (destTile == sourceTile) {
    newDest = sourceTile + 1;
    cashDelta = newDest;
    scoreDelta = newDest;
  } else {

    switch (board[dest].tileType) {
      case negativeLawsuit:
        newDest = sourceTile - 1;
        break;
      case positiveLawsuit:
        newDest = sourceTile + 1;
        break;
      default:
        newDest = sourceTile;
    }

    const auto bonusValue = newBoard->bonus[dest];

    if (bonusValue) {
      cashDelta = bonusValue;
      scoreDelta = bonusValue;
      newBoard->bonus[dest] = 0;
    } else if (newBoard->isLawsuit(dest)) {
      cashDelta = 0;
      scoreDelta = 0;
    } else {
      cashDelta = -1;
      scoreDelta = 0;
    }
  }

  // Competitor costs must be calculated before adding the new competitor
  newBoard->cash += cashDelta;
  newBoard->cash -= newBoard->competitorCosts();

  if (nextTile.tileType == competitor) {
    newBoard->addCompetitor(source, nextTile);
  } else {
    newBoard->board[source] = nextTile;
  }

  newBoard->board[dest] = Tile(newDest);
  newBoard->score += scoreDelta;

  return newBoard;
}

BoardPtr Board::jump(
        const int source,
        const int dest,
        const Tile& nextTile,
        const int start,
        const int dist,
        const bool horizontalMove) const {
  const int sourceTile = this->board[source].value;

  BoardPtr newBoard = std::make_shared<Board>(*this);

  newBoard->board[source] = Tile();
  newBoard->board[dest] = Tile(sourceTile + 1);
  newBoard->cash += sourceTile + 1;
  newBoard->score += sourceTile + 1;

  int destroyedTiles = 0;
  for (int i=1; i<dist; i++) {
    const int pos = horizontalMove ? start + i : (start + i*5);
    const int val = newBoard->board[pos].value;

    if (!newBoard->isCompetitor(pos) && !newBoard->isNonProfit(pos)) continue;

    if (sourceTile > val) {
      if (newBoard->isCompetitor(pos)) newBoard->clearCompetitor(pos);
      else newBoard->board[pos] = Tile();

      destroyedTiles++;
    }
  }

  if (destroyedTiles > 1) {
    const int comboBonus = 1 << destroyedTiles;
    newBoard->score += comboBonus;
    newBoard->cash += comboBonus;
  }

  // Competitor costs must come after competitors are eliminated
  newBoard->cash -= newBoard->competitorCosts();

  return newBoard;
}

BoardPtr Board::move(
        const int source,
        const int dest,
        const Tile& nextTile) {
  int start, dist;

  const int x1 = source % 5;
  const int y1 = source / 5;
  const int x2 = dest % 5;
  const int y2 = dest / 5;

  bool horizontalMove = y1 == y2;

  if (horizontalMove) {
    start = x1 < x2 ? source : dest;
    dist = abs(x1 - x2);
  } else {
    start = y1 < y2 ? source : dest;
    dist = abs(y1 - y2);
  }

  auto newBoard = (dist == 1) ? \
                    this->walk(source, dest, nextTile) : \
                    this->jump(source, dest, nextTile, start, dist, horizontalMove);

  newBoard->updateBonus();
  newBoard->updateTimer();

  return newBoard;
}

const Tile Board::getRandomTile(int score) {
  using std::cout;

  float p = (rand()/static_cast<float>(RAND_MAX));

  const int distribRow = std::min(score/100, PROBABILITY_INTERVALS-1);
  int tileIndex = 0;

  while (true) {
    p -= DISTRIBUTION[distribRow][tileIndex];
    if (p > 0) tileIndex++;
    else break;
  }

  if (tileIndex >= TILE_TYPES) {
    cout << "DEBUG: tileIndex = " << tileIndex << '\n';
    return Tile(1);
  }

  return TILES[tileIndex];
}

