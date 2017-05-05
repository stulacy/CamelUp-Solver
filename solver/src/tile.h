#ifndef TILE_H 
#define TILE_H 

class TileOccupant;
class CamelStack;

class Tile {
    public:
        Tile(int);
        void setOccupant(TileOccupant);
        TileOccupant getOccupant(void);
        bool isEmpty(void);
        void onCamelLand(CamelStack);
        
    private:
        int num;
        TileOccupant* occupant;
        
};

#endif
