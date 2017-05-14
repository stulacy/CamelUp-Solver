#include "tile.h"

Tile::Tile(int id) : id(id), occupant(nullptr) {};

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

int Tile::add_camel_stack(CamelStack* cam, bool reverse) {
    int ret;
    if (isEmpty()) {
        Rcpp::Rcout << "Empty tile so creating new stack\n";
        occupant = dynamic_cast<TileOccupant*>(cam);
        ret = 0;
    } else {
        Rcpp::Rcout << "Tile has occupant so will add this camel stack to existing stack\n";
        ret = occupant->add_cam_stack(cam->getCamels(), reverse);
    } 
    return ret;
}
