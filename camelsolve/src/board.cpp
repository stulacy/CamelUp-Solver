#include <Rcpp.h>
#include "board.h"

Board::Board(Rcpp::IntegerMatrix state) {
    for (int i=0; i < state.nrow(); ++i) {
        std::unique_ptr<TileOccupant> occupant;
        // Find camel stacks
        std::vector<std::pair<int, int>> raw_camels;
        for (int j=0; j < 5; ++j) {
           if (state(i, j) > 0) {
                raw_camels.emplace_back(state(i, j), j);
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
            
            occupant = std::unique_ptr<TileOccupant>(new CamelStack(camels));
        } else if (state(i, 5) == 1) {
            occupant = std::unique_ptr<TileOccupant>(new ForwardTrap());
        } else if (state(i, 6) == 1) {
            occupant = std::unique_ptr<TileOccupant>(new BackwardTrap());
        } else {
            occupant = nullptr;
        }
        
        if (occupant == nullptr) {
            tiles.push_back(Tile(i));
        } else {
            tiles.push_back(Tile(i, std::move(occupant)));
        }
    }
    
    // Add 3 empty tiles for winner to land on, as could be on tile 16 and roll a 3
    tiles.push_back(Tile(16));
    tiles.push_back(Tile(17));
    tiles.push_back(Tile(18));
}

std::vector<int> Board::get_camelstack(int tile_index, int camel_num) {
    Tile* tile = &tiles.at(tile_index);
    TileOccupant* camstack = tile->getOccupant();
    
    // TODO Shouldn't have to use this cast hack. Indicates design flaw
    if (dynamic_cast<CamelStack*>(camstack) != NULL) {
        std::vector<int>& curr_camels = dynamic_cast<CamelStack*>(camstack)->getCamels();
        std::vector<int> new_camels (curr_camels.size());
        std::copy(curr_camels.begin(), curr_camels.end(), new_camels.begin());
        
        int pos_in_stack, i;
        
        auto it = curr_camels.begin();
        for (i=0; it < curr_camels.end(); ++it, ++i) {
            if ((*it) == camel_num) {
                pos_in_stack = i;
                break;
            }
        }
        
        // If position is 0, then delete the entire camel stack, i.e. set this tile occupant to NULL
        if (pos_in_stack == 0) {
            tile->setOccupant(nullptr);
        } else {
            // For original stack delete all values >= pos
            curr_camels.erase(curr_camels.begin()+pos_in_stack, curr_camels.end());
            
            // For new stack go through new vector and delete all values < pos
            new_camels.erase(new_camels.begin(), new_camels.begin()+pos_in_stack);
        }
        
        return new_camels;
    
    } else {
        // Should never get here
        Rcpp::Rcerr << "Error: TileOccupant isn't coerced to CamelStack \n";
        return std::vector<int>(2);
    }
}

int Board::get_camel_from_stack(int tile_index, bool from_top /* = true */) {
    Tile* tile = &tiles.at(tile_index);
    TileOccupant* camstack = tile->getOccupant();
    // TODO Shouldn't have to use this cast hack. Indicates design flaw
    int camel_id;
    if (dynamic_cast<CamelStack*>(camstack) != NULL) {
        std::vector<int>& curr_camels = dynamic_cast<CamelStack*>(camstack)->getCamels();
        camel_id = (from_top) ? curr_camels.back() : curr_camels.front();
    } else {
        Rcpp::Rcerr << "Can't find camel stack at index " << tile_index << ".\n";
        camel_id = 0;
    }
    
    return camel_id;
}

int Board::move_camels(std::vector<int> camels, int location, bool reverse /* = false */) {
    // Pass items into recursive function to add to camel stack, return the tile id
    // of the tile on which it lands
    Tile* tile = &tiles.at(location);
    int tile_output = tile->add_camel_stack(camels, reverse);
    
    if (tile_output == 0) {
        return location;
    } else {
        return move_camels(camels, location + tile_output, tile_output==-1);
    }
}
