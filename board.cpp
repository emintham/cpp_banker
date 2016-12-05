#include <iostream>
#include <map>
#include <tuple>
#include <vector>

#include "constants.h"
#include "board.h"

using namespace std;

// Cache for reused heuristic values
typedef map<int, int> HeuristicCache;
HeuristicCache hc;

// Initialize static member variables
int Board::reusedValues = 0;
bool Board::markReuse = true;


int hashBoard(const Board &b) {
  int h = 25;
  for (int i=0; i<25; i++) {
    h = h * 17 + b.board[i];
  }

  return h;
}

//
// Board methods
//
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

bool Board::isEmpty(int position) {
  return !board[position] && !competitors[position];
}

bool Board::isCompetitor(int position) {
  return competitors[position];
}

bool Board::isBankrupt() {
  return cash < 0;
}

int Board::heuristicScore() {
  int heuristicScore = 0;

  // Look for it in cache
  int key = hashBoard(*this);
  map<int, int>::iterator it = hc.find(key);

  if (it != hc.end()) {
    if (markReuse) {
      reusedValues++;
    }
    return it->second + cash;
  }

  // Bankruptcy is not related to board position, don't cache
  if (this->isBankrupt()) return BANKRUPT;

  /*
  const int edges[12] = {1, 2, 3, 5, 9, 10, 14, 15, 19, 21, 22, 23};
  const int centers[9] = {6, 7, 8, 11, 12, 13, 16, 17, 18};
  */

  for (const tuple<int, int> &move : this->getMoveset()) {
    int s, d;
    tie(s, d) = move;

    int diff = abs(s - d);
    int dist = diff/5 + diff%5;
    int val = dist > 1 ? board[s] : 1;

    heuristicScore += val + (1 << (dist - 1));
  }

  /*
  // Corners
  if (this->isCompetitor(0)) heuristicScore -= CORNER_PENALTY;
  if (this->isCompetitor(4)) heuristicScore -= CORNER_PENALTY;
  if (this->isCompetitor(20)) heuristicScore -= CORNER_PENALTY;
  if (this->isCompetitor(24)) heuristicScore -= CORNER_PENALTY;

  for (int i=0; i<12; i++) {
    if (this->isCompetitor(edges[i])) {
      heuristicScore -= EDGE_PENALTY;
    }
  }

  for (int i=0; i<9; i++) {
    int cost = centers[i];
    if (this->isCompetitor(cost)) {
      heuristicScore += board[cost];
    }
  }
  */

  // Add to hashtable
  hc[key] = heuristicScore;

  return heuristicScore + cash;
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

float Board::bestMove(int nextTile, int depth, int* source, int* dest) {
  if (depth == 0) {
    return this->heuristicScore();
  }

  int chosenSource = -1;
  int chosenDest = -1;
  float bestScore = 0.0;

  if (this->isBankrupt()) return BANKRUPT;

  vector<tuple<int, int>> allPossibleMoves = this->getMoveset();

  if (allPossibleMoves.empty()) return BANKRUPT;

  for (const tuple<int, int> &move : allPossibleMoves) {
    int s, d;
    tie(s, d) = move;

    Board* nextBoard = this->move(s, d, nextTile);
    float score = Board::EMM(nextBoard, depth-1);

    if (score > bestScore) {
      chosenSource = s;
      chosenDest = d;
      bestScore = score;
    }

    delete nextBoard;
  }

  if (chosenSource < 0) {
    return BANKRUPT;
  } else {
    *source = chosenSource;
    *dest = chosenDest;

    return bestScore;
  }
}

float Board::EMM(Board* board, int depth) {
  if (depth == 0 ) return board->heuristicScore();

  if (board->isBankrupt()) return BANKRUPT;

  int score = board->score;
  vector<pair<int, float>> tileWithProbabilities;

  if (board->score <= 300) {
    tileWithProbabilities.push_back(make_pair(2, 0.35));
    tileWithProbabilities.push_back(make_pair(1, 0.35));
    tileWithProbabilities.push_back(make_pair(0, 0.2));
    tileWithProbabilities.push_back(make_pair(-1, 0.09));
    tileWithProbabilities.push_back(make_pair(-2, 0.01));
  } else if (board->score <= 500) {
    tileWithProbabilities.push_back(make_pair(2, 0.23));
    tileWithProbabilities.push_back(make_pair(1, 0.27));
    tileWithProbabilities.push_back(make_pair(0, 0.23));
    tileWithProbabilities.push_back(make_pair(-1, 0.09));
    tileWithProbabilities.push_back(make_pair(-2, 0.09));
    tileWithProbabilities.push_back(make_pair(-3, 0.09));
  } else {
    tileWithProbabilities.push_back(make_pair(2, 0.21));
    tileWithProbabilities.push_back(make_pair(1, 0.23));
    tileWithProbabilities.push_back(make_pair(0, 0.27));
    tileWithProbabilities.push_back(make_pair(-1, 0.01));
    tileWithProbabilities.push_back(make_pair(-2, 0.09));
    tileWithProbabilities.push_back(make_pair(-3, 0.09));
    tileWithProbabilities.push_back(make_pair(-4, 0.09));
    tileWithProbabilities.push_back(make_pair(-5, 0.01));
  }

  float expectedMaxScore = 0.0;
  for (const pair<int, float> &tileProbabilityPair : tileWithProbabilities) {
    int tile, source, dest;
    float probability, heuristicScore;
    tie(tile, probability) = tileProbabilityPair;

    heuristicScore = board->bestMove(tile, depth-1, &source, &dest);

    expectedMaxScore += heuristicScore * probability;
  }

  return expectedMaxScore;
}
