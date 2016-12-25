#include <benchmark/benchmark_api.h>

#include "board.h"
#include "tile.h"

BoardPtr setUp() {
  BoardPtr b = std::make_shared<Board>();

  b->board = {Tile(7), Tile(4),             Tile(2),                  Tile(4),             Tile(7),
              Tile(6), Tile(3, competitor), Tile(1),                  Tile(3, nonProfit),  Tile(6),
              Tile(3), Tile(2),             Tile(0, positiveLawsuit), Tile(2),             Tile(3),
              Tile(6), Tile(3, nonProfit),  Tile(1),                  Tile(3, competitor), Tile(6),
              Tile(7), Tile(4),             Tile(2),                  Tile(4),             Tile(7)};
  b->addCompetitor(6, Tile(3, competitor));
  b->addCompetitor(18, Tile(3, competitor));

  return b;
}

static void BM_setUpBoard(benchmark::State& state) {
  while (state.KeepRunning()) {
    auto b = setUp();
  }
}
BENCHMARK(BM_setUpBoard);

static void BM_createTile(benchmark::State& state) {
  while (state.KeepRunning()) {
    Tile(1, nonProfit);
  }
}
BENCHMARK(BM_createTile);

static void BM_walk(benchmark::State& state) {
  while (state.KeepRunning()) {
    auto b = setUp();
    b->move(12, 11, Tile(1));
  }
}
BENCHMARK(BM_walk);

static void BM_jump(benchmark::State& state) {
  while (state.KeepRunning()) {
    auto b = setUp();
    b->move(10, 14, Tile(1));
  }
}
BENCHMARK(BM_jump);

static void BM_competitorCosts(benchmark::State& state) {
  auto b = setUp();

  while (state.KeepRunning()) {
    b->competitorCosts();
  }
}
BENCHMARK(BM_competitorCosts);

static void BM_isEmpty(benchmark::State& state) {
  auto b = setUp();

  while (state.KeepRunning()) {
    b->isEmpty(0);
  }
}
BENCHMARK(BM_isEmpty);

static void BM_getMoveset(benchmark::State& state) {
  auto b = setUp();

  while (state.KeepRunning()) {
    b->getMoveset();
  }
}
BENCHMARK(BM_getMoveset);

BENCHMARK_MAIN();
