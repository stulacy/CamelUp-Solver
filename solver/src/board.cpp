#include <Rcpp.h>
#include "board.h"

Board::Board(Rcpp::IntegerMatrix state) {
    for (int i=0; i < 16; ++i) {
        Tile tile;
        
        // Could definitely refactor this
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
    TileOccupant* camstack = tile.getOccupant();
    // TODO Shouldn't have to use this hack. Indicates design flaw
    if (dynamic_cast<CamelStack*>(camstack) != NULL) {
        std::vector<int>& curr_camels = dynamic_cast<CamelStack*>(camstack)->getCamels();
        std::vector<int> new_camels (curr_camels.size());
        std::copy(curr_camels.begin(), curr_camels.end(), new_camels.begin());
        
        int pos_in_stack, i;
        auto it = curr_camels.begin();
        
        for (i=0; it < curr_camels.end(); ++it, ++i) {
            Rcpp::Rcout << "On iteration: " << i << "\n";
            if ((*it) == camel_num) {
                Rcpp::Rcout << "Found camel looking for\n";
                pos_in_stack = i;
                break;
            }
        }
        
        // If position is 0, then delete the entire camel stack, i.e. set this tile occupant to NULL
        if (pos_in_stack == 0) {
            Rcpp::Rcout << "At bottom of stack so am deleting entire TileOccupant \n";
            // TODO What to put here?
            //delete camstack;
            tile.setOccupant(nullptr);
        } else {
            // For original stack delete all values >= pos
            curr_camels.erase(curr_camels.begin()+pos_in_stack, curr_camels.end());
            
            // For new stack go through new vector and delete all values < pos
            new_camels.erase(new_camels.begin(), new_camels.begin()+pos_in_stack);
            
            Rcpp::Rcout << "Original stack: \n";
            for (auto it = curr_camels.begin(); it < curr_camels.end(); ++it) {
                Rcpp::Rcout << (*it) << ",";
            }
            Rcpp::Rcout << "\n";
        }
        
        Rcpp::Rcout << "New stack: \n";
        for (auto it = new_camels.begin(); it < new_camels.end(); ++it) {
            Rcpp::Rcout << (*it) << ",";
        }
        Rcpp::Rcout << "\n";
        
        return new CamelStack(new_camels);
    
    } else {
        // Should never get here
        Rcpp::Rcerr << "Error: TileOccupant isn't coerced to CamelStack \n";
        return new CamelStack(std::vector<int>(2));
    }
}
