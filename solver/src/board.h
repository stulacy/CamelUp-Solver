#ifndef BOARD_H 
#define BOARD_H 
#include <Rcpp.h>
#include <vector>

class Tile;

class Board {
    public:
        Board(Rcpp::IntegerMatrix state, Rcpp::LogicalVector dice);
        
    private:
        std::vector<Tile*> tiles;
        std::vector<bool> dice;
};

#endif
