#include <Rcpp.h>
#include "board.h"

Board::Board(Rcpp::IntegerMatrix state) {
    for (int i=0; i < 16; ++i) {
        Tile tile;
        
        // TODO Could definitely refactor this
        // Find camel stacks
        std::vector<std::pair<int, int>> raw_camels;
        for (int j=0; j < 5; ++j) {
           if (state(i, j) > 0) {
                raw_camels.emplace_back(state(i, j), j);
                Rcpp::Rcout << "Found camel " << j << " in order " << state(i, j) << "\n";
            }
        }
        
        if (raw_camels.size() > 0) {
            std::sort(begin(raw_camels), end(raw_camels), 
                      [](std::pair<int, int> const &t1, std::pair<int, int> const &t2) {
                          return std::get<0>(t1) < std::get<0>(t2); // or use a custom compare function
                      }
            );
            // TODO No one liner for this?
            std::vector<int> camels;
            for (auto it = raw_camels.begin(); it != raw_camels.end(); ++it) {
                camels.push_back(std::get<1>(*it));
            }
            
            tile.setOccupant(new CamelStack(camels));
        } else if (state(i, 5) == 1) {
            tile.setOccupant(new ForwardTrap());
        } else if (state(i, 6) == 1) {
            tile.setOccupant(new BackwardTrap());
        }
        
        tiles.push_back(tile);
        Rcpp::Rcout << "Added tile to tiles. Now size " << tiles.size() << " \n";
    }
}

Tile Board::getTile(int index) {
    return tiles[index];
}

CamelStack* Board::getCamel(int tile_index, int camel_num) {
    Tile tile = tiles[tile_index];
    //return tile.getOccupant();
}
