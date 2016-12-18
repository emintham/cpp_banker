#include <iostream>
#include <tuple>
#include <vector>

#include "constants.h"
#include "board.h"

using namespace std;

void Board::print() {
  for (int i=0; i<BOARD_SIZE; i++) {
    if (i != 0 && i % 5 == 0) {
      cout << '\n';
    }

    if (this->isCompetitor(i)) {
      if (board[i] == 0) {
        cout << "( 0)";
      } else {
        cout << "(" << board[i] << ")";
      }
    } else {
      cout << "  " << board[i] << " ";
    }
  }

  cout << '\n';
}

void Board::printMove(int source, int dest) {
  for (int i=0; i<BOARD_SIZE; i++) {
    if (i != 0 && i % 5 == 0) {
      cout << '\n';
    }

    if (i == source) {
      cout << "A ";
    } else if (i == dest) {
      cout << "B ";
    } else {
      cout << "+ ";
    }
  }
  cout << '\n';
}


bool Board::isEmpty(int position) {
  return !board[position] && !this->isCompetitor(position);
}

bool Board::isCompetitor(int position) {
  return competitors.test(position);
}

bool Board::isBankrupt() {
  return cash < 0;
}

int Board::competitorCosts() {
  int total = 0;

  for (auto& num: board) {
    total += num > 0 ? 0 : num;
  }

  return total;
}

vector<tuple<int, int, int>> Board::getMoveset() {
  vector<tuple<int, int, int>> allPossibleMoves;

  for (int i=0; i<BOARD_SIZE; i++) {
    if (board[i] <= 0) continue;

    for (const int& j : possibleDest[i]) {
      int diff = abs(i-j);
      int dist = diff/5 + diff%5;

      bool walk = dist == 1 && this->isEmpty(j);
      bool jump = dist != 1 && board[i] == board[j];

      if (walk || jump) {
        allPossibleMoves.push_back(make_tuple(i, j, dist));
      }
    }
  }

  return allPossibleMoves;
}

void Board::addCompetitor(int pos, int value) {
  board[pos] = value;
  competitors[pos] = 1;
  competitorTimers[pos] = 20;
}

void Board::clearCompetitor(int pos) {
  board[pos] = 0;
  competitors[pos] = 0;
  competitorTimers[pos] = 0;
}

void Board::addBonus(int pos, int value) {
  bonus[pos] = value;
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

Board* Board::move(int source, int dest, int nextTile) {
  int start, dist;

  int x1 = source % 5;
  int y1 = source / 5;
  int x2 = dest % 5;
  int y2 = dest / 5;
  int sourceTile = this->board[source];

  bool horizontalMove = y1 == y2;

  if (horizontalMove) {
    start = x1 < x2 ? source : dest;
    dist = abs(x1 - x2);
  } else {
    start = y1 < y2 ? source : dest;
    dist = abs(y1 - y2);
  }

  int newDest, cashDelta, scoreDelta, newSource;

  if (dist == 1) {
    newDest = sourceTile;
    cashDelta = -1;
    scoreDelta = 0;
  } else {
    newDest = sourceTile+1;
    cashDelta = newDest;
    scoreDelta = newDest;
  }

  newSource = dist == 1 ? nextTile : 0;

  Board* newBoard = new Board(*this);

  int destroyedCompetitors = 0;
  for (int i=1; i<dist; i++) {
    int pos = horizontalMove ? start + i : (start + i*5);
    int val = newBoard->board[pos];

    if (newBoard->isCompetitor(pos)) {
      if (val == 0 || (val < 0 && sourceTile + val > 0)) {
        newBoard->clearCompetitor(pos);
        destroyedCompetitors++;
      }
    }
  }

  if (destroyedCompetitors > 1) {
    int delta = 1 << destroyedCompetitors;
    newBoard->score += delta;
    newBoard->cash += delta;
  }

  newBoard->cash += newBoard->competitorCosts() + cashDelta;

  newBoard->board[source] = newSource;
  newBoard->board[dest] = newDest;
  newBoard->score += scoreDelta;

  if (dist == 1) {
    if (nextTile <= 0) {
      newBoard->addCompetitor(source, nextTile);
      newBoard->updateTimer();
    }

    auto bonusValue = bonus[dest];

    if (bonusValue) {
      newBoard->cash += bonusValue;
      newBoard->bonus[dest] = 0;
    }
  }

  // This has to happen after the bonus calculation
  newBoard->updateBonus();

  return newBoard;
}

int Board::getRandomTile(int score) {
  float p = (rand()/static_cast<float>(RAND_MAX));
  const float *prob_ptr = &DISTRIBUTION[0][0];
  const int *tile_ptr = &TILES[0];
  const int *end = TILES + TILE_TYPES;

  if (score <= 300) {
    // do nothing
  } else if (score <= 600) {
    prob_ptr += TILE_TYPES;
  } else {
    prob_ptr += TILE_TYPES << 1;
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

