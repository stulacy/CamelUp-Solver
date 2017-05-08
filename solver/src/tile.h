#ifndef TILE_H 
#define TILE_H 

class TileOccupant;

class Tile {
    public:
        Tile();
        void setOccupant(TileOccupant*);
        TileOccupant* getOccupant(void);
        bool isEmpty(void);
        
    private:
        TileOccupant* occupant;
        
};

#endif
