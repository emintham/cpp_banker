#include <benchmark/benchmark_api.h>

#include "board.h"

static void benchmark_getMoveset(benchmark::State& state) {
  BoardPtr b = std::make_shared<Board>();

  b->board = {Tile(7), Tile(4),             Tile(2),                  Tile(4),             Tile(7),
              Tile(6), Tile(3, competitor), Tile(1),                  Tile(3, nonProfit),  Tile(6),
              Tile(3), Tile(2),             Tile(0, positiveLawsuit), Tile(2),             Tile(3),
              Tile(6), Tile(3, nonProfit),  Tile(1),                  Tile(3, competitor), Tile(6),
              Tile(7), Tile(4),             Tile(2),                  Tile(4),             Tile(7)};
  b->addCompetitor(6, Tile(3, competitor));
  b->addCompetitor(18, Tile(3, competitor));

  while (state.KeepRunning()) {
    b->getMoveset();
  }
}
BENCHMARK(benchmark_getMoveset);

BENCHMARK_MAIN();
