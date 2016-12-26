#include <algorithm>
#include <iostream>
#include <memory>

#include "board.h"
#include "tile.h"

void report(const BoardPtr& b, const Tile& tile) {
  using std::cout;

#ifdef DEBUG
  cout << *b;
#else
  for (const auto& t: b->board) {
    cout << t << ' ';
  }
  cout << '\n';
#endif

  cout << b->score << ' ' << b->cash << '\n';

  for (const auto& move: b->getMoveset()) {
    int s, d, dist;
    std::tie(s, d, dist) = move;

    cout << s << ',' << d << ' ';
  }
  cout << '\n';

  cout << tile << '\n';
}

void bankerLoop(const bool interactive) {
  BoardPtr b = std::make_shared<Board>();
  int src, dst;

  while (true) {
    const auto randomTile = b->getRandomTile(b->score);
    int dist = 10;

    do {
      report(b, randomTile);
      if (scanf("%d %d", &src, &dst) == EOF) return;

      b = b->move(src, dst, randomTile);

      const int diff = abs(src-dst);
      dist = diff/5 + diff%5;
    } while (dist > 1);

    if (b->isBankrupt()) {
      report(b, Tile());
      break;
    }
  }
}

int main(int argc, const char* argv[]) {
  bool interactive = false;

  if (argc == 2) {
    std::string s (argv[1]);
    interactive = s.compare("-i") == 0;
  }

  bankerLoop(interactive);

  return 0;
}
