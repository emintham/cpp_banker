#include <iostream>
#include <string>
#include <memory>

#include <time.h>

#include "board.h"
#include "tile.h"
#include "emm.h"

template<class T>
std::string formatWithCommas(const T& t) {
  std::string numWithCommas (std::to_string(t));
  int insertPosition = numWithCommas.length() - 3;

  while (insertPosition > 0) {
    numWithCommas.insert(insertPosition, ",");
    insertPosition -= 3;
  }

  return numWithCommas;
}

int main(int argc, const char* argv[]) {
  int depth = 6;

  int dist;
  BoardPtr b = std::make_shared<Board>();
  std::shared_ptr<EMM> emm = std::make_shared<EMM>();
  emm->countLeafNodes = true;

  if (argc == 2) {
    depth = std::stoi(argv[1]);
  }

  b->board = {Tile(7), Tile(4),             Tile(2),                  Tile(4),             Tile(7),
              Tile(6), Tile(3, competitor), Tile(1),                  Tile(3, nonProfit),  Tile(6),
              Tile(3), Tile(2),             Tile(0, positiveLawsuit), Tile(2),             Tile(3),
              Tile(6), Tile(3, nonProfit),  Tile(1),                  Tile(3, competitor), Tile(6),
              Tile(7), Tile(4),             Tile(2),                  Tile(4),             Tile(7)};
  b->addCompetitor(6, Tile(3, competitor));
  b->addCompetitor(18, Tile(3, competitor));

  clock_t t = clock();  // Start recording

  emm->solveBestMove(b, Tile(5, competitor), depth, &dist, false);

  t = clock() - t;      // End recording

  // Print numbers nicely with commas
  std::cout << "Explored to a depth of " << depth;
  std::cout << ", nodes = " << formatWithCommas(emm->leafNodesExplored) << '\n';
  std::cout << "Took " << ((float)t)/CLOCKS_PER_SEC << " secs" << "\n\n";

  return 0;
}
