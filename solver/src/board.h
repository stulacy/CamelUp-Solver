#ifndef BOARD_H 
#define BOARD_H 
#include <Rcpp.h>
#include <vector>
#include "camelstack.h"
#include "forwardtrap.h"
#include "backwardtrap.h"
#include "tile.h"


class Board {
    public:
        Board(Rcpp::IntegerMatrix state);
        CamelStack* getCamel(int, int);
        int move_camels(CamelStack*, int, bool=false);
        Tile getTile(int);
        
    private:
        std::deque<Tile> tiles;
};

#endif
