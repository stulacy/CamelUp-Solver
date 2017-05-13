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

int Tile::add_camel_stack(CamelStack* cam) {
    int ret;
    TileOccupant* camel_pointer;
    Rcpp::Rcout << "tile::add_camel_stack BEFORE\ttile " << this << "\toccupant: " << occupant << "\tisEmpty(): " << isEmpty() << "\n";
    if (isEmpty()) {
        //setOccupant(dynamic_cast<TileOccupant*>(cam));
        camel_pointer = dynamic_cast<TileOccupant*>(cam);
        occupant = camel_pointer;
        Rcpp::Rcout << "tile::add_camel_stack AFTER\ttile " << this << "\toccupant: " << occupant << "\tisEmpty(): " << isEmpty() << "\n";
        ret = 0;
    } else {
        Rcpp::Rcout << "Tile has occupant so will add it\n";
        ret = occupant->add_cam_stack(cam->getCamels());
    } 
    return ret;
}
