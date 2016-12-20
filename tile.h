#ifndef __TILE_H__
#define __TILE_H__

enum TileType {
  regular,
  nonProfit,
  negativeLawsuit,
  positiveLawsuit
};

struct Tile {
  int value;
  TileType tileType;

  Tile(int value)
      : value(value), tileType(regular) {}

  Tile(int value, TileType tileType)
      : value(value), tileType(tileType) {}
};

#endif
