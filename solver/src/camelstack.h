#include "tileoccupant.h"
#include <vector>

class CamelStack : TileOccupant {
    public: 
        CamelStack(std::vector<int>);
    
    private:
        std::vector<int> camels;
        
};
