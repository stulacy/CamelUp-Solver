#include "tile.h"

Tile::Tile() : occupant(nullptr) {};

bool Tile::isEmpty(void) {
    if (occupant == nullptr) {
        return true;
    } else {
        return false;
    }
}

void Tile::setOccupant(TileOccupant* occ) {
    occupant = occ;
}

TileOccupant* Tile::getOccupant(void) {
    return occupant;
}
