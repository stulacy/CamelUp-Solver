#ifndef BOARD_H 
#define BOARD_H 
#include <Rcpp.h>
#include <vector>

class Tile;
class CamelStack;

class Board {
    public:
        Board(Rcpp::IntegerMatrix state);
        CamelStack getCamelStack(int);
        
    private:
        std::vector<Tile*> tiles;
        std::vector<bool> dice;
};

#endif
