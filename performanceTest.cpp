#include <iostream>

#include <time.h>

#include "board.h"
#include "tile.h"
#include "emm.h"

int main() {
  int dist;
  const int depth = 6;
  BoardPtr b = std::make_shared<Board>();

  b->board = {Tile(7), Tile(4),             Tile(2),                  Tile(4),             Tile(7),
              Tile(6), Tile(3, competitor), Tile(1),                  Tile(3, nonProfit),  Tile(6),
              Tile(3), Tile(2),             Tile(0, positiveLawsuit), Tile(2),             Tile(3),
              Tile(6), Tile(3, nonProfit),  Tile(1),                  Tile(3, competitor), Tile(6),
              Tile(7), Tile(4),             Tile(2),                  Tile(4),             Tile(7)};
  b->addCompetitor(6, Tile(3, competitor));
  b->addCompetitor(18, Tile(3, competitor));

  clock_t t = clock();  // Start recording

  EMM::solveBestMove(b, Tile(5, competitor), depth, &dist, false);

  t = clock() - t;      // End recording

  std::cout << "Explored to a depth of " << depth << '\n';
  std::cout << "Took " << ((float)t)/CLOCKS_PER_SEC << " secs" << "\n\n";

  return 0;
}
