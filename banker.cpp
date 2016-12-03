#include <array>
#include <cmath>
#include <iostream>
#include <set>
#include <tuple>
#include <vector>

#include <time.h>

using namespace std;

const int CORNER_PENALTY = 30;
const int EDGE_PENALTY = 10;
const float BANKRUPT = -10000.0;

const int possibleDest[25][8] = {
  {1, 2, 3, 4, 5, 10, 15, 20}, // 0
  {0, 2, 3, 4, 6, 11, 16, 21}, // 1
  {0, 1, 3, 4, 7, 12, 17, 22}, // 2
  {0, 1, 2, 4, 8, 13, 18, 23}, // 3
  {0, 1, 2, 3, 9, 14, 19, 24}, // 4
  {0, 6, 7, 8, 9, 10, 15, 20}, // 5
  {1, 5, 7, 8, 9, 11, 16, 21}, // 6
  {2, 5, 6, 8, 9, 12, 17, 22}, // 7
  {3, 5, 6, 7, 9, 13, 18, 23}, // 8
  {4, 5, 6, 7, 8, 14, 19, 24}, // 9
  {0, 5, 11, 12, 13, 14, 15, 20}, // 10
  {1, 6, 10, 12, 13, 14, 16, 21}, // 11
  {2, 7, 10, 11, 13, 14, 17, 22}, // 12
  {3, 8, 10, 11, 12, 14, 18, 23}, // 13
  {4, 9, 10, 11, 12, 13, 19, 24}, // 14
  {0, 5, 10, 16, 17, 18, 19, 20}, // 15
  {1, 6, 11, 15, 17, 18, 19, 21}, // 16
  {2, 7, 12, 15, 16, 18, 19, 22}, // 17
  {3, 8, 13, 15, 16, 17, 19, 23}, // 18
  {4, 9, 14, 15, 16, 17, 18, 24}, // 19
  {0, 5, 10, 15, 21, 22, 23, 24}, // 20
  {1, 6, 11, 16, 20, 22, 23, 24}, // 21
  {2, 7, 12, 17, 20, 21, 23, 24}, // 22
  {3, 8, 13, 18, 20, 21, 22, 24}, // 23
  {4, 9, 14, 19, 20, 21, 22, 23}  // 24
};

class Board {

public:
  array<int, 25> board = {0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0,
                          0, 0, 1, 0, 0,
                          0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0};
  bool competitors[25] = {false};
  int moves = 0;
  int score = 10;
  int cash = 10;


  float bestMove(int nextTile, int depth, int* source, int* dest);
  Board* move(int source, int dest, int nextTile);
  void print();
  bool isEmpty(int position);
  bool isCompetitor(int position);
  static float EMM(Board* board, int depth);
  float competitorCosts();
  bool isBankrupt();

private:
  float heuristicScore();
  vector<tuple<int, int>> getMoveset();
};

void Board::print() {
  for (int i=0; i<25; i+=5) {
    cout << board[i] << " " << board[i+1] << " " << board[i+2] << " ";
    cout << board[i+3] << " " << board[i+4] << endl;
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

float Board::heuristicScore() {
  if (this->isBankrupt()) return BANKRUPT;

  float heuristicScore = score + cash;

  const int edges[12] = {1, 2, 3, 5, 9, 10, 14, 15, 19, 21, 22, 23};
  const int centers[9] = {6, 7, 8, 11, 12, 13, 16, 17, 18};

  for (const tuple<int, int> &move : this->getMoveset()) {
    int s, d;
    tie(s, d) = move;

    int diff = abs(s - d);
    int dist = diff/5 + diff%5;
    int val = board[s];

    heuristicScore += dist > 1 ? (val * dist) : val;
  }

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

  return heuristicScore;
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

  for (int i=1; i<dist; i++) {
    int pos = horizontalMove ? start + i : (start + i*5);

    if (newBoard->isCompetitor(pos) && newBoard->board[pos] < 0 && sourceTile + newBoard->board[pos] > 0) {
      newBoard->board[pos] = 0;
      newBoard->competitors[pos] = false;
    }
  }

  newBoard->board[source] = newSource;
  newBoard->board[dest] = newDest;

  if (dist == 1 && nextTile <= 0) {
    newBoard->competitors[source] = true;
  }

  newBoard->cash += newBoard->competitorCosts() + cashDelta;
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

int main() {

  int nextTile, source, dest;
  float score;
  int depth = 6;

  Board* b = new Board();

  while (scanf("%d", &nextTile) != EOF) {

    retry: // FOR GOTO

    clock_t t = clock();  // Start recording

    score = b->bestMove(nextTile, depth, &source, &dest);

    t = clock() - t;      // End recording

    Board* newBoard = b->move(source, dest, nextTile);

    cout << "Next tile: " << nextTile << endl;
    cout << "Source: " << source << ", " << "Dest: " << dest;
    cout << ", Score: " << newBoard->score << ", Cash: " << newBoard->cash;
    cout << endl;
    cout << "Heuristic: " << score << endl;

    newBoard->print();

    cout << "Took " << ((float)t)/CLOCKS_PER_SEC << " secs" << endl << endl;

    delete b;
    b = newBoard;

    int diff = abs(source - dest);
    int dist = diff/5 + diff%5;

    if (dist > 1) goto retry;
  }

  return 0;

}
