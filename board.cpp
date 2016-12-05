#include <iostream>
#include <tuple>
#include <vector>

#include "constants.h"
#include "board.h"

using namespace std;

void Board::print() {
  for (int i=0; i<25; i++) {
    if (i != 0 && i % 5 == 0) {
      cout << endl;
    }

    if (competitors[i]) {
      if (board[i] == 0) {
        cout << "( 0)";
      } else {
        cout << "(" << board[i] << ")";
      }
    } else {
      cout << "  " << board[i] << " ";
    }
  }

  cout << endl;
}

void Board::printMove(int source, int dest) {
  for (int i=0; i<25; i++) {
    if (i != 0 && i % 5 == 0) {
      cout << endl;
    }

    if (i == source) {
      cout << "A ";
    } else if (i == dest) {
      cout << "B ";
    } else {
      cout << "+ ";
    }
  }
  cout << endl;
}


bool Board::isEmpty(int position) {
  return !board[position] && !competitors[position];
}

bool Board::isCompetitor(int position) {
  return competitors[position];
}

bool Board::isBankrupt() {
  return cash < 0;
}

float Board::competitorCosts() {
  float total= 0.0;

  for (int i=0; i<25; i++) {
    int value = board[i];

    if (value < 0) {
      total += value;
    }
  }

  return total;
}

vector<tuple<int, int>> Board::getMoveset() {
  vector<tuple<int, int>> allPossibleMoves;

  for (int i=0; i<25; i++) {
    if (board[i] <= 0) continue;

    for (const int& j : possibleDest[i]) {
      int diff = abs(i-j);
      bool isAdjacent = (diff / 5 + diff % 5) == 1;

      if (diff / 5 + diff % 5 == 1) {
        if (this->isEmpty(j)) {
          allPossibleMoves.push_back(make_tuple(i, j));
        }
      } else {
        if (board[i] == board[j]) {
          allPossibleMoves.push_back(make_tuple(i, j));
        }
      }
    }
  }

  return allPossibleMoves;
}

void Board::addCompetitor(int pos, int value) {
  board[pos] = value;
  competitors[pos] = true;
  competitorTimers[pos] = 20;
}

void Board::clearCompetitor(int pos) {
  board[pos] = 0;
  competitors[pos] = false;
  competitorTimers[pos] = 0;
}

void Board::updateTimer() {
  for (int i=0; i<25; i++) {
    if (!competitors[i]) continue;

    if (!competitorTimers[i]) {
      board[i]--;
      competitorTimers[i] = 20;
      continue;
    }

    competitorTimers[i]--;
  }
}

Board* Board::move(int source, int dest, int nextTile) {
  int start, dist;

  int x1 = source % 5;
  int y1 = source / 5;
  int x2 = dest % 5;
  int y2 = dest / 5;
  int sourceTile = this->board[source];
  int destTile = this->board[dest];

  bool horizontalMove = y1 == y2;

  if (horizontalMove) {
    start = x1 < x2 ? source : dest;
    dist = abs(x1 - x2);
  } else {
    start = y1 < y2 ? source : dest;
    dist = abs(y1 - y2);
  }

  int newDest, cashDelta, scoreDelta, newSource;

  if (sourceTile != destTile) {
    newDest = sourceTile;
    cashDelta = -1;
    scoreDelta = 0;
  } else {
    newDest = sourceTile+1;
    cashDelta = newDest;
    scoreDelta = newDest;
  }

  if (dist == 1) {
    newSource = nextTile;
  } else {
    newSource = 0;
  }

  Board* newBoard = new Board(*this);

  int destroyedCompetitors = 0;
  for (int i=1; i<dist; i++) {
    int pos = horizontalMove ? start + i : (start + i*5);

    if (newBoard->isCompetitor(pos) && newBoard->board[pos] < 0 && sourceTile + newBoard->board[pos] > 0) {
      newBoard->board[pos] = 0;
      newBoard->competitors[pos] = false;
      destroyedCompetitors++;
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

  if (dist == 1 && nextTile <= 0) {
    newBoard->addCompetitor(source, nextTile);
    newBoard->updateTimer();
  }

  newBoard->score += scoreDelta;
  newBoard->moves++;

  return newBoard;
}
