#include "tile.h"

Tile::Tile(int id, std::unique_ptr<TileOccupant> occ) : id(id), occupant(std::move(occ)) {}
Tile::Tile(int id) : id(id) {}

bool Tile::isEmpty(void) {
    return (occupant == nullptr);
}

void Tile::setOccupant(std::unique_ptr<TileOccupant> occ) {
    occupant = std::move(occ);
}

TileOccupant* Tile::getOccupant(void) {
    return occupant.get();
}

int Tile::add_camel_stack(std::vector<int>camels, bool reverse) {
    int ret;
    if (isEmpty()) {
        Rcpp::Rcout << "Adding camels to empty tile\n";
        setOccupant(std::unique_ptr<TileOccupant>(new CamelStack(camels)));
        ret = 0;
    } else {
        ret = occupant->add_cam_stack(camels, reverse);
    } 
    return ret;
}
