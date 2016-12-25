#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <vector>

#include <time.h>
#include <stdlib.h>

#include "constants.h"
#include "emm.h"
#include "tile.h"

// Cache for reused heuristic values
typedef std::map<int, int> HeuristicCache;
static HeuristicCache hc;

int hashBoard(const Board &b) {
  int h = BOARD_SIZE;
  for (int i=0; i<BOARD_SIZE; i++) {
    h = h * 17 + b.board[i].value;
  }

  return h;
}

BoardPtr EMM::solveBestMove(
        const BoardPtr& b,
        const Tile& nextTile,
        int depth,
        int* dist,
        bool verbose) {
  using std::cout;

  clock_t t = clock();  // Start recording

  int source, dest;
  this->bestMove(b, nextTile, depth, &source, &dest);

  if (source < 0 || dest < 0) {
    cout << "Failed!\n";
    return nullptr;
  }

  t = clock() - t;      // End recording

  auto newBoard = b->move(source, dest, nextTile);

  if (verbose) {
    // cout << "Next tile: " << nextTile.value << '\n';
    cout << "Score: " << newBoard->score << ", Cash: " << newBoard->cash << '\n';

    Board::printMove(source, dest);

    cout << std::string(50, '-') << '\n';

    cout << "Took " << ((float)t)/CLOCKS_PER_SEC << " secs" << "\n\n";
  }

  const int diff = abs(source - dest);
  *dist = diff/5 + diff%5;

  return newBoard;
}

BoardPtr EMM::solveBestMove(
        const BoardPtr& b,
        const Tile& nextTile,
        int depth,
        int* dist) {
  return this->solveBestMove(b, nextTile, depth, dist, true);
}

BoardPtr EMM::handleLawsuit(
        std::istringstream& currentLine,
        std::ofstream& tileFile,
        const BoardPtr& b,
        const int depth) {
  char c;
  int dist = 0;
  BoardPtr newBoard = b;

  currentLine >> c;
  const Tile tile = Tile(0, (c == '-') ? negativeLawsuit : positiveLawsuit);

  tileFile << c << ' ' << b->score << '\n';

  do {
    newBoard = this->solveBestMove(newBoard, tile, depth, &dist);
  } while (dist > 1);

  return newBoard;
}

BoardPtr EMM::handleBonus(
        std::istringstream& currentLine,
        std::ofstream& tileFile,
        const BoardPtr& b,
        const int depth) {
  int cash, pos;

  currentLine >> cash >> pos;

  tileFile << '$' << cash << ' ' << b->score << '\n';

  b->addBonus(pos, cash);

  return b;
}

BoardPtr EMM::handleNonProfit(
        std::istringstream& currentLine,
        std::ofstream& tileFile,
        const BoardPtr& b,
        const int depth) {
  int nonProfitValue, dist;
  BoardPtr newBoard = b;

  currentLine >> nonProfitValue;

  tileFile << '.' << nonProfitValue << ' ' << b->score << '\n';

  do {
    newBoard = this->solveBestMove(newBoard, Tile(nonProfitValue, nonProfit), depth, &dist);
  } while (dist > 1);

  return newBoard;
}

void EMM::handleDebug(
        std::istringstream& currentLine,
        std::ofstream& tileFile,
        const BoardPtr& b) {
  std::string command;
  currentLine >> command;

  if (command == "ct") {
    b->printCompetitorTimers();
  }
}

BoardPtr EMM::handleTile(
        const int nextTile,
        std::ofstream& tileFile,
        const BoardPtr& b,
        const int depth) {
  int dist;
  BoardPtr newBoard = b;

  // Record the tiles and score to file
  tileFile << nextTile << " " << b->score << '\n';

  Tile t;
  if (nextTile > 0) {
    t = Tile(nextTile);
  } else {
    t = Tile(-nextTile, competitor);
  }

  do {
    newBoard = this->solveBestMove(newBoard, t, depth, &dist);
  } while (dist > 1);

  return newBoard;
}

void EMM::commandParser(int depth) {
  std::ofstream myfile ("tiles.txt", std::ios_base::app);
  std::string line;

  BoardPtr b = std::make_shared<Board>();

  while (getline(std::cin, line)){
    std::istringstream iss (line);

    char c;
    iss >> c;

    switch (c) {
      case '$': {
        b = this->handleBonus(iss, myfile, b, depth);
        break;
      }
      case '!': {
        b = this->handleLawsuit(iss, myfile, b, depth);
        break;
      }
      case 'p': {
        std::cout << *b;
        break;
      }
      case '.': {
        b = this->handleNonProfit(iss, myfile, b, depth);
        break;
      }
      case 'd': {
        this->handleDebug(iss, myfile, b);
        break;
      }
      default: {
        const int nextTile = stoi(line);
        b = this->handleTile(nextTile, myfile, b, depth);
        break;
      }
    }
  }

  myfile.close();
}

int EMM::heuristicScore(const BoardPtr& b) {
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

  // Add to hashtable
  hc[key] = heuristicScore;
  */

  return heuristicScore + b->cash + b->score + BOARD_SIZE - b->numCompetitors();
}

float EMM::bestMove(
        const BoardPtr& b,
        const Tile& nextTile,
        int depth,
        int* source,
        int* dest) {

  *source = -1;
  *dest = -1;

  if (depth == 0 || b->isBankrupt()) {
    if (countLeafNodes) leafNodesExplored++;
    return this->heuristicScore(b);
  }

  int chosenSource = -1;
  int chosenDest = -1;
  float bestScore = 0.0;
  const bool isNonProfit = nextTile.tileType == nonProfit;
  const bool isCompetitor = nextTile.tileType == competitor;

  const auto allPossibleMoves = b->getMoveset();

  if (allPossibleMoves.empty()) {
    if (countLeafNodes) leafNodesExplored++;
    return this->heuristicScore(b);
  }

  for (const auto &move : allPossibleMoves) {
    int s, d, dist;
    std::tie(s, d, dist) = move;

    // Do not recommend moves where the competitor or nonProfit ends up in the
    // corner
    const bool badTile = isNonProfit || isCompetitor;
    const bool isCorner = s == 0 || s == 4 || s == 20 || s == 24;

    if (badTile && isCorner) continue;

    auto nextBoard = b->move(s, d, nextTile);
    const float score = this->expectiminimax(nextBoard, depth-1);

    if (score > bestScore) {
      chosenSource = s;
      chosenDest = d;
      bestScore = score;
    }
  }

  *source = chosenSource;
  *dest = chosenDest;

  if (chosenSource < 0) {
    if (countLeafNodes) leafNodesExplored++;
    return this->heuristicScore(b);
  } else {
    return bestScore;
  }
}

float EMM::expectiminimax(const BoardPtr& board, int depth) {
  if (depth == 0 || board->isBankrupt()) {
    if (countLeafNodes) leafNodesExplored++;
    return this->heuristicScore(board);
  }

  const int distribRow = std::min(board->score/100, PROBABILITY_INTERVALS-1);

  float expectedMaxScore = 0.0;
  for (int i=0; i<TILE_TYPES; i++) {
    int source, dest;

    const Tile tile = TILES[i];
    const float probability = DISTRIBUTION[distribRow][i];

    const float heuristicScore = this->bestMove(board, tile, depth-1, &source, &dest);

    expectedMaxScore += heuristicScore * probability;
  }

  return expectedMaxScore;
}

int EMM::rolloutOnce(int depth) {
  BoardPtr b = std::make_shared<Board>();

  while (true) {
    int dist = 10;
    const Tile newTile = Board::getRandomTile(b->score);

    do {
      b = this->solveBestMove(b, newTile, depth, &dist, false);
      // std::cout << '.';

      if (!b) return 0;
      std::cout << "Score: " << b->score << ", Cash: " << b->cash << '\n';
      // std::cout << *b;

    } while (dist > 1);
  }

  int score = b->score;
  std::cout << "\nScore: " << b->score << ", Cash: " << b->cash << '\n';

  return score;
}

void EMM::rollout(int depth) {
  const int numRollouts = 6;

  srand(time(0));

  for (int i=0; i<numRollouts; i++) {
    this->rolloutOnce(depth);
  }
}
