#include <iostream>
#include <map>
#include <tuple>
#include <vector>

#include <time.h>

#include "constants.h"
#include "emm.h"

using namespace std;

// Initialize static member variables
int EMM::reusedValues = 0;
bool EMM::markReuse = true;


// Cache for reused heuristic values
typedef map<int, int> HeuristicCache;
static HeuristicCache hc;

int hashBoard(const Board &b) {
  int h = 25;
  for (int i=0; i<25; i++) {
    h = h * 17 + b.board[i];
  }

  return h;
}

void EMM::solveBestMove() {
  int nextTile;
  float score;
  int source = -1;
  int dest = -1;
  int depth = 6;

  Board* b = new Board();

  while (scanf("%d", &nextTile) != EOF) {

    retry: // FOR GOTO

    if (markReuse) {
      reusedValues = 0;
    }

    clock_t t = clock();  // Start recording

    score = EMM::bestMove(b, nextTile, depth, &source, &dest);

    if (source < 0 || dest < 0) {
      cout << "Failed!" << endl;
      break;
    }

    t = clock() - t;      // End recording

    Board* newBoard = b->move(source, dest, nextTile);

    cout << "Next tile: " << nextTile << endl;
    cout << "Score: " << newBoard->score << ", Cash: " << newBoard->cash;
    cout << endl;
    cout << "Heuristic: " << score;

    if (markReuse) {
      cout << ", " << "Reused values: " << reusedValues;
    }
    cout << endl;

    Board::printMove(source, dest);

    cout << string(50, '-') << endl;

    newBoard->print();

    cout << "Took " << ((float)t)/CLOCKS_PER_SEC << " secs" << endl << endl;

    delete b;
    b = newBoard;

    int diff = abs(source - dest);
    int dist = diff/5 + diff%5;

    if (dist > 1) goto retry;
  }

}

int EMM::getRandomTile(int score) {
  int tiles[8] = {2, 1, 0, -1, -2, -3, -4, -5};
  float distribution[3][8] = {
    {0.35, 0.35, 0.20, 0.09, 0.01, 0.00, 0.00, 0.00},
    {0.23, 0.27, 0.23, 0.09, 0.09, 0.09, 0.00, 0.00},
    {0.21, 0.23, 0.27, 0.01, 0.09, 0.09, 0.09, 0.01}
  };

  float p = (rand()/static_cast<float>(RAND_MAX));
  float *prob_ptr = &distribution[0][0];
  int *tile_ptr = &tiles[0];
  int *end = tiles + 8;

  if (score <= 300) {
    // do nothing
  } else if (score <= 600) {
    prob_ptr += 8;
  } else {
    prob_ptr += 16;
  }

  while((p -= *prob_ptr) > 0) {
    ++prob_ptr;
    ++tile_ptr;
  }

  if (tile_ptr > end) {
    cout << "DEBUG: past end of array!" << endl;
    return 0;
  }

  return *tile_ptr;
}

void EMM::getMaxScore() {
  Board *b = new Board();

  int sumScore = 0;
  int runs = 0;
  int score = 0;
  int source = -1;
  int dest = -1;
  int depth = 6;
  int maxRuns = 5;

  while (true) {
    if (runs >= maxRuns) break;

    int nextTile = getRandomTile(b->score);

    retry: // FOR GOTO

    if (markReuse) {
      reusedValues = 0;
    }

    clock_t t = clock();  // Start recording

    score = EMM::bestMove(b, nextTile, depth, &source, &dest);

    if (score < 0) {
      b->print();
      break;
    }

    if (source < 0 || dest < 0 || b->cash < 0) {
      runs++;
      sumScore += b->score;

      delete b;
      b = new Board();

      continue;
    }

    t = clock() - t;      // End recording

    Board* newBoard = b->move(source, dest, nextTile);

    cout << "Source: " << source << ", Dest: " << dest << endl;
    cout << "Score: " << newBoard->score << ", Cash: " << newBoard->cash;
    cout << endl;
    cout << "Heuristic: " << score;

    if (markReuse) {
      cout << ", " << "Reused values: " << reusedValues;
    }
    cout << endl;

    cout << "Took " << ((float)t)/CLOCKS_PER_SEC << " secs" << endl << endl;

    delete b;
    b = newBoard;

    int diff = abs(source - dest);
    int dist = diff/5 + diff%5;

    if (dist > 1) goto retry;

  }

  if (runs) {
    cout << "Average score across " << runs << " runs: ";
    cout << (float)sumScore/(float)runs << endl;
  }
}

int EMM::heuristicScore(Board *b) {
  int heuristicScore = 0;

  // Look for it in cache
  int key = hashBoard(*b);
  map<int, int>::iterator it = hc.find(key);

  if (it != hc.end()) {
    if (markReuse) {
      reusedValues++;
    }
    return it->second + b->cash;
  }

  // Bankruptcy is not related to board position, don't cache
  if (b->isBankrupt()) return BANKRUPT;

  /*
  const int edges[12] = {1, 2, 3, 5, 9, 10, 14, 15, 19, 21, 22, 23};
  const int centers[9] = {6, 7, 8, 11, 12, 13, 16, 17, 18};
  */

  for (const tuple<int, int> &move : b->getMoveset()) {
    int s, d;
    tie(s, d) = move;

    int diff = abs(s - d);
    int dist = diff/5 + diff%5;
    int val = dist > 1 ? b->board[s] : 1;

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

  return heuristicScore + b->cash;
}

float EMM::bestMove(Board *b, int nextTile, int depth, int* source, int* dest) {
  if (depth == 0) {
    return EMM::heuristicScore(b);
  }

  int chosenSource = -1;
  int chosenDest = -1;
  float bestScore = 0.0;

  if (b->isBankrupt()) return BANKRUPT;

  vector<tuple<int, int>> allPossibleMoves = b->getMoveset();

  if (allPossibleMoves.empty()) return BANKRUPT;

  for (const tuple<int, int> &move : allPossibleMoves) {
    int s, d;
    tie(s, d) = move;

    Board* nextBoard = b->move(s, d, nextTile);
    float score = EMM::expectiminimax(nextBoard, depth-1);

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

float EMM::expectiminimax(Board* board, int depth) {
  if (depth == 0 ) return EMM::heuristicScore(board);

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

    heuristicScore = EMM::bestMove(board, tile, depth-1, &source, &dest);

    expectedMaxScore += heuristicScore * probability;
  }

  return expectedMaxScore;
}
