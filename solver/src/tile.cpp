#include "tile.h"
#include "camelstack.h"

Tile::Tile(int num): num(num), occupant() {}

bool Tile::isEmpty(void) {
    if (occupant) {
        return true;
    } else {
        return false;
    }
}

void Tile::onCamelLand(CamelStack camel) {
    // TODO If is empty then set occupant as camel stack
    
    // TODO Otherwise, run TileOccupant.
}
