#ifndef OCCUPANT_H 
#define OCCUPANT_H 
#include <iostream>

class TileOccupant {
    public: 
        TileOccupant(void);
        virtual std::string getName() = 0;
};

#endif
