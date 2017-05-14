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
        std::vector<int> getCamel(int, int);
        int move_camels(std::vector<int>, int, bool=false);
        
    private:
        std::deque<Tile> tiles;
};

#endif
