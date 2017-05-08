#ifndef CAMELSTACK_H 
#define CAMELSTACK_H 
#include "tileoccupant.h"
#include <vector>

class CamelStack : public TileOccupant {
    public: 
        CamelStack(std::vector<int>);
        std::vector<int> getCamels(void) const {return camels;};
        std::string getName();
    
    private:
        std::vector<int> camels;
};

#endif
