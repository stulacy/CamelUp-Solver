#ifndef TILE_H 
#define TILE_H 

#include "tileoccupant.h"
#include "camelstack.h"

class Tile {
    public:
        Tile(int);
        void setOccupant(TileOccupant*);
        TileOccupant* getOccupant(void);
        bool isEmpty(void);
        int add_camel_stack(CamelStack*, bool);
        
    private:
        TileOccupant* occupant;
        int id;
};

#endif
