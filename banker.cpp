#include <iostream>
#include <time.h>

#include "board.h"
#include "constants.h"

using namespace std;

void printMove(int source, int dest) {
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

void solveBestMove() {
  int nextTile;
  float score;
  int source = -1;
  int dest = -1;
  int depth = 6;

  Board* b = new Board();

  while (scanf("%d", &nextTile) != EOF) {

    retry: // FOR GOTO

    if (b->markReuse) {
      b->reusedValues = 0;
    }

    clock_t t = clock();  // Start recording

    score = b->bestMove(nextTile, depth, &source, &dest);

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

    if (b->markReuse) {
      cout << ", " << "Reused values: " << b->reusedValues;
    }
    cout << endl;

    printMove(source, dest);

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

int getRandomTile(int score) {
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

void getMaxScore() {
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

    if (b->markReuse) {
      b->reusedValues = 0;
    }

    clock_t t = clock();  // Start recording

    score = b->bestMove(nextTile, depth, &source, &dest);

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

    if (b->markReuse) {
      cout << ", " << "Reused values: " << b->reusedValues;
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

int main() {

  // solveBestMove();
  getMaxScore();

  return 0;

}
