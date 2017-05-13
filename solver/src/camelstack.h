#ifndef CAMELSTACK_H 
#define CAMELSTACK_H 
#include "tileoccupant.h"
#include <vector>

class CamelStack : public TileOccupant {
    public: 
        CamelStack(std::vector<int>);
        std::vector<int>& getCamels(void) {return camels;};
        std::string getName();
        int add_cam_stack(std::vector<int>);
    
    private:
        std::vector<int> camels;
};

#endif
