#ifndef TILE_H 
#define TILE_H 

#include "tileoccupant.h"
#include "camelstack.h"
#include <memory>

class Tile {
    public:
        Tile(int, std::unique_ptr<TileOccupant>);
        Tile(int);
        bool isEmpty(void);
        void setOccupant(std::unique_ptr<TileOccupant>);
        TileOccupant* getOccupant(void);
        int add_camel_stack(std::vector<int>, bool);
        
    private:
        std::unique_ptr<TileOccupant> occupant;
        const int id;
};

#endif
