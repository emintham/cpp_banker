#include <iostream>
#include <fstream>
#include <map>
#include <stack>
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
  int h = BOARD_SIZE;
  for (int i=0; i<BOARD_SIZE; i++) {
    h = h * 17 + b.board[i];
  }

  return h;
}

void EMM::solveBestMove() {
  int nextTile;
  int pos;
  float score;
  int source = -1;
  int dest = -1;
  int depth = 6;

  ofstream myfile ("tiles.txt", std::ios_base::app);

  Board* b = new Board();

  while (scanf("%d %d", &nextTile, &pos) != EOF) {

    if (pos > 0) {
      b->addBonus(pos, nextTile);
      continue;
    }

    // Record the tiles and score to file
    myfile << nextTile << " " << b->score << '\n';

    retry: // FOR GOTO

    if (markReuse) {
      reusedValues = 0;
    }

    clock_t t = clock();  // Start recording

    score = EMM::bestMove(b, nextTile, depth, &source, &dest);

    if (source < 0 || dest < 0) {
      cout << "Failed!\n";
      break;
    }

    t = clock() - t;      // End recording

    Board* newBoard = b->move(source, dest, nextTile);

    cout << "Next tile: " << nextTile << '\n';
    cout << "Score: " << newBoard->score << ", Cash: " << newBoard->cash << '\n';
    cout << "Heuristic: " << score;

    if (markReuse) {
      cout << ", " << "Reused values: " << reusedValues;
    }
    cout << '\n';

    Board::printMove(source, dest);

    cout << string(50, '-') << '\n';

    // newBoard->print();

    cout << "Took " << ((float)t)/CLOCKS_PER_SEC << " secs" << "\n\n";

    delete b;
    b = newBoard;

    int diff = abs(source - dest);
    int dist = diff/5 + diff%5;

    if (dist > 1) goto retry;
  }

  myfile.close();
}

void EMM::getMaxScore() {
  Board *b = new Board();

  int sumScore = 0;
  int runs = 0;
  float score = 0;
  int source = -1;
  int dest = -1;
  int depth = 7;
  int maxRuns = 5;

  while (true) {
    if (runs >= maxRuns) break;

    int nextTile = Board::getRandomTile(b->score);

    retry: // FOR GOTO

    if (markReuse) {
      reusedValues = 0;
    }

    clock_t t = clock();  // Start recording

    score = EMM::bestMove(b, nextTile, depth, &source, &dest);

    if (score < 0 || source < 0 || dest < 0 || b->cash < 0) {
      b->print();

      runs++;
      sumScore += b->score;

      delete b;
      b = new Board();

      continue;
    }

    t = clock() - t;      // End recording

    Board* newBoard = b->move(source, dest, nextTile);

    // cout << "Source: " << source << ", Dest: " << dest << '\n';
    cout << "Score: " << newBoard->score << ", Cash: " << newBoard->cash << '\n';
    cout << "Heuristic: " << score;

    if (markReuse) {
      cout << ", " << "Reused values: " << reusedValues;
    }
    cout << '\n';

    b->print();

    cout << "Took " << ((float)t)/CLOCKS_PER_SEC << " secs" << "\n\n";

    delete b;
    b = newBoard;

    int diff = abs(source - dest);
    int dist = diff/5 + diff%5;

    if (dist > 1) goto retry;

  }

  if (runs) {
    cout << "Average score across " << runs << " runs: ";
    cout << (float)sumScore/(float)runs << '\n';
  }
}

int EMM::heuristicScore(Board *b) {
  int heuristicScore = 0;

  /*
  // Look for it in cache
  int key = hashBoard(*b);
  map<int, int>::iterator it = hc.find(key);

  if (it != hc.end()) {
    if (markReuse) {
      reusedValues++;
    }
    return it->second + b->cash + b->score;
  }
  */

  for (int i=0; i<BOARD_SIZE; i++) {
    // heuristicScore += (int)b->competitors[i];
    // if (!b->isCompetitor(i)) heuristicScore += 1;
    heuristicScore += b->competitors.size() - b->competitors.count();
  }

  /*
  // Add to hashtable
  hc[key] = heuristicScore;
  */

  return heuristicScore + b->cash + b->score;
}

float EMM::bestMove(Board *b, int nextTile, int depth, int* source, int* dest) {
  if (depth == 0) {
    return EMM::heuristicScore(b);
  }

  int chosenSource = -1;
  int chosenDest = -1;
  float bestScore = 0.0;

  if (b->isBankrupt()) return BANKRUPT;

  vector<tuple<int, int, int>> allPossibleMoves = b->getMoveset();

  if (allPossibleMoves.empty()) return BANKRUPT;

  for (auto &move : allPossibleMoves) {
    int s, d, dist;
    tie(s, d, dist) = move;

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

  int j = 0;

  if (board->score <= 300) {
    // Do nothing
  } else if (board->score <= 500) {
    j = 1;
  } else {
    j = 2;
  }

  float expectedMaxScore = 0.0;
  for (int i=0; i<TILE_TYPES; i++) {
    int source, dest;

    int tile = TILES[i];
    float probability = DISTRIBUTION[j][i];

    float heuristicScore = EMM::bestMove(board, tile, depth-1, &source, &dest);

    expectedMaxScore += heuristicScore * probability;
  }

  return expectedMaxScore;
}
