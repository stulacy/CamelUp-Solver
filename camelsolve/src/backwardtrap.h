#ifndef BACKWARDTRAP_H 
#define BACKWARDTRAP_H 
#include "tileoccupant.h"
#include <vector>

class BackwardTrap : public TileOccupant {
    public: 
        BackwardTrap();
        std::string getName();
        int add_cam_stack(std::vector<int>, bool);
};

#endif
