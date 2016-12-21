#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <iomanip>
#include <vector>

#include "constants.h"
#include "board.h"

void Board::print() const {
  using std::cout;

  for (int i=0; i<BOARD_SIZE; i++) {
    if (i != 0 && i % 5 == 0) {
      cout << '\n';
    }

    if (this->isCompetitor(i)) {
      if (board[i] == 0) {
        cout << "(0)";
      } else {
        cout << ' ' << board[i];
      }
    } else if (negativeLawsuits[i]) {
      cout << " - ";
    } else if (positiveLawsuits[i]) {
      cout << " + ";
    } else {
      const int bonusVal = bonus[i];

      cout << (bonusVal ? '$' : ' ') << (bonusVal ? bonusVal : board[i]);
      cout << (nonProfits[i] ? '*' : ' ');
    }
  }

  cout << '\n';
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
  return !board[position] && !this->isCompetitor(position);
}

bool Board::isCompetitor(int position) const {
  return competitors.test(position);
}

bool Board::isBankrupt() const {
  return cash < 0;
}

int Board::competitorCosts() const {
  int total = 0;

  for (auto& num: board) {
    total += num > 0 ? 0 : num;
  }

  return total;
}

std::vector<std::tuple<int, int, int>> Board::getMoveset() const {
  std::vector<std::tuple<int, int, int>> allPossibleMoves;

  for (int i=0; i<BOARD_SIZE; i++) {
    if (board[i] <= 0 || nonProfits[i]) continue;

    for (const int& j : possibleDest[i]) {
      int diff = abs(i-j);
      int dist = diff/5 + diff%5;

      // If source is a lawsuit, it can only walk to an adjacent tile
      if (negativeLawsuits[i] || positiveLawsuits[i]) {
        if (dist == 1 && !this->isEmpty(j)) {
          allPossibleMoves.push_back(std::make_tuple(i, j, dist));
        }

        continue;
      }

      bool walk = dist == 1 && !nonProfits[j] && (this->isEmpty(j) || board[i] == board[j]);
      bool jump = dist != 1 && board[i] == board[j] && !nonProfits[j];

      if (walk || jump) {
        allPossibleMoves.push_back(std::make_tuple(i, j, dist));
      }
    }
  }

  return allPossibleMoves;
}

void Board::addCompetitor(int pos, int value) {
  board[pos] = value;
  competitors[pos] = 1;
  competitorTimers[pos] = 19;
}

void Board::clearCompetitor(int pos) {
  board[pos] = 0;
  competitors[pos] = 0;
  competitorTimers[pos] = 0;
}

void Board::addBonus(int pos, int value) {
  bonus[pos] = value;
}

void Board::addLawsuit(int pos, TileType tileType) {
  switch(tileType) {
    case negativeLawsuit:
      negativeLawsuits[pos] = true;
      break;
    case positiveLawsuit:
      positiveLawsuits[pos] = true;
      break;
    default:
      break;
  }
}

void Board::addNonProfit(int pos, int value) {
  nonProfits[pos] = true;
  board[pos] = value;
}

void Board::updateTimer() {
  for (int i=0; i<BOARD_SIZE; i++) {
    if (!this->isCompetitor(i)) continue;

    if (!competitorTimers[i]) {
      board[i]--;
      competitorTimers[i] = 20;
      continue;
    }

    competitorTimers[i]--;
  }
}

void Board::updateBonus() {
  for (int i=0; i<BOARD_SIZE; i++) {
    if (bonus[i]) {
      bonus[i]--;
    }
  }
}

Board* Board::move(
        int source,
        int dest,
        const Tile& nextTile) {
  int start, dist;

  const int x1 = source % 5;
  const int y1 = source / 5;
  const int x2 = dest % 5;
  const int y2 = dest / 5;
  const int sourceTile = this->board[source];
  const int destTile = this->board[dest];

  bool horizontalMove = y1 == y2;

  if (horizontalMove) {
    start = x1 < x2 ? source : dest;
    dist = abs(x1 - x2);
  } else {
    start = y1 < y2 ? source : dest;
    dist = abs(y1 - y2);
  }

  Board* newBoard = new Board(*this);

  if (negativeLawsuits[source]) {
    newBoard->board[dest]--;
    newBoard->negativeLawsuits[source] = false;
    newBoard->updateTimer();
    newBoard->updateBonus();

    return newBoard;
  }

  if (positiveLawsuits[source]) {
    newBoard->board[dest]++;
    newBoard->positiveLawsuits[source] = false;
    newBoard->updateTimer();
    newBoard->updateBonus();

    return newBoard;
  }

  int newDest, cashDelta, scoreDelta, newSource;

  if (destTile == sourceTile) {
    newDest = sourceTile + 1;
    cashDelta = newDest;
    scoreDelta = newDest;
  } else {

    if (negativeLawsuits[dest]) {
      newDest = sourceTile - 1;
      newBoard->negativeLawsuits[dest] = false;
    } else if (positiveLawsuits[dest]) {
      newDest = sourceTile + 1;
      newBoard->positiveLawsuits[dest] = false;
    } else {
      newDest = sourceTile;
    }

    cashDelta = -1;
    scoreDelta = 0;
  }

  newSource = dist == 1 ? nextTile.value : 0;

  int destroyedCompetitors = 0;
  for (int i=1; i<dist; i++) {
    const int pos = horizontalMove ? start + i : (start + i*5);
    const int val = newBoard->board[pos];

    if (newBoard->isCompetitor(pos)) {
      if (sourceTile + val > 0) {
        newBoard->clearCompetitor(pos);
        destroyedCompetitors++;
      } else {
        // Add the value of the competitor back to cash since jumping over a
        // competitor stops it from stealing from you that turn even if you
        // don't eliminate it
        newBoard->cash += -val;
      }
    } else if (newBoard->nonProfits[pos] && val < sourceTile) {
      newBoard->board[pos] = 0;
      newBoard->nonProfits[pos] = false;
      destroyedCompetitors++;
    }
  }

  if (destroyedCompetitors > 1) {
    const int delta = 1 << destroyedCompetitors;
    newBoard->score += delta;
    newBoard->cash += delta;
  }

  newBoard->cash += newBoard->competitorCosts() + cashDelta;

  newBoard->board[source] = newSource;
  newBoard->board[dest] = newDest;
  newBoard->score += scoreDelta;

  if (dist == 1) {
    switch (nextTile.tileType) {
      case nonProfit:
        newBoard->nonProfits[source] = true;
        break;

      case negativeLawsuit:
        newBoard->addLawsuit(source, negativeLawsuit);
        break;

      case positiveLawsuit:
        newBoard->addLawsuit(source, positiveLawsuit);
        break;

      default:
        if (nextTile.value <= 0) {
          newBoard->addCompetitor(source, nextTile.value);
        }
    }

    const auto bonusValue = newBoard->bonus[dest];

    if (bonusValue) {
      // We add 1 because you are not charged for the new tile if moving to a
      // bonus square
      newBoard->cash += bonusValue + 1;
      newBoard->score += bonusValue;
      newBoard->bonus[dest] = 0;
    }
  }

  // This has to happen after the adding the competitor and bonus calculations
  newBoard->updateBonus();
  newBoard->updateTimer();

  return newBoard;
}

int Board::getRandomTile(int score) {
  using std::cout;

  float p = (rand()/static_cast<float>(RAND_MAX));
  const float *prob_ptr = &DISTRIBUTION[0][0];
  const int *tile_ptr = &TILES[0];
  const int *end = TILES + TILE_TYPES;

  if (score >= 100) {
    const int jumpLength = std::max(score / 100 - 1, PROBABILITY_INTERVALS-1);
    cout << "DEBUG: Score: " << score << ", Jump: " << jumpLength << '\n';
    prob_ptr += TILE_TYPES << jumpLength;
  }

  while((p -= *prob_ptr) > 0) {
    ++prob_ptr;
    ++tile_ptr;
  }

  if (tile_ptr > end) {
    cout << "DEBUG: past end of array!\n";
    return 0;
  }

  return *tile_ptr;
}

