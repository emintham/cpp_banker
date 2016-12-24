#ifndef __TILE_H__
#define __TILE_H__

#include <ostream>

enum TileType {
  regular,
  nonProfit,
  negativeLawsuit,
  positiveLawsuit,
  competitor
};

struct Tile {
  int value;
  TileType tileType;

  Tile()
      : value(0), tileType(regular) {}

  Tile(int value)
      : value(value), tileType(regular) {}

  Tile(int value, TileType tileType)
      : value(value), tileType(tileType) {}

  bool operator==(const Tile& t) const {
    return value == t.value && tileType == t.tileType;
  }

  bool operator!=(const Tile& t) const {
    return value != t.value || tileType != t.tileType;
  }

  bool operator<(const Tile& t) const {
    return value < t.value;
  }

  bool operator>(const Tile& t) const {
    return value > t.value;
  }

  bool operator<=(const Tile& t) const {
    return value <= t.value;
  }

  bool operator>=(const Tile& t) const {
    return value >= t.value;
  }

  friend std::ostream& operator<<(std::ostream& os, const Tile t) {
    switch (t.tileType) {
      case regular:
        os << ' ' << t.value << ' ';
        break;
      case nonProfit:
        os << ' ' << t.value << '*';
        break;
      case negativeLawsuit:
        os << " - ";
        break;
      case positiveLawsuit:
        os << " + ";
        break;
      case competitor:
        if (t.value > 0) os << '-' << t.value << ' ';
        else os << "(0)";
        break;
    }

    return os;
  }

};

#endif
