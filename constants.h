#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "tile.h"

const int BOARD_SIZE = 25;

const int CORNER_PENALTY = 30;
const int EDGE_PENALTY = 10;
const int BANKRUPT = -1;

const int NUM_CORNERS = 4;
const int NUM_EDGES = 12;
const int NUM_CENTERS = 9;

const int CORNERS[NUM_CORNERS] = {0, 4, 20, 24};
const int EDGES[NUM_EDGES] = {1, 2, 3, 5, 9, 10, 14, 15, 19, 21, 22, 23};
const int CENTERS[NUM_CENTERS] = {6, 7, 8, 11, 12, 13, 16, 17, 18};

const int possibleDest[BOARD_SIZE][8] = {
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

const int PROBABILITY_INTERVALS = 6;
const int TILE_TYPES = 10;
const Tile TILES[TILE_TYPES] = {
  Tile(2),
  Tile(1),
  Tile(0, competitor),
  Tile(1, competitor),
  Tile(2, competitor),
  Tile(3, competitor),
  Tile(2, nonProfit),
  Tile(3, nonProfit),
  Tile(0, positiveLawsuit),
  Tile(0, negativeLawsuit)
};

const float DISTRIBUTION[PROBABILITY_INTERVALS][TILE_TYPES] = {
  //   2,     1,     0,    -1,    -2,    -3,    .2,    .3      +,     -
  {0.428, 0.373, 0.175, 0.024, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000}, // score < 100
  {0.337, 0.393, 0.192, 0.078, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000}, // score < 200
  {0.322, 0.357, 0.193, 0.110, 0.013, 0.000, 0.000, 0.000, 0.000, 0.000}, // score < 300
  {0.208, 0.370, 0.195, 0.156, 0.032, 0.000, 0.013, 0.006, 0.006, 0.013}, // score < 400
  {0.309, 0.309, 0.134, 0.155, 0.062, 0.010, 0.000, 0.010, 0.000, 0.010}, // score < 500
  {0.412, 0.235, 0.059, 0.176, 0.118, 0.000, 0.000, 0.000, 0.000, 0.000}, // else
};

#endif
