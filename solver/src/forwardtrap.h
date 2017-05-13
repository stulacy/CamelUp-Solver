#ifndef FORWARDTRAP_H 
#define FORWARDTRAP_H 
#include "tileoccupant.h"
#include <vector>

class ForwardTrap : public TileOccupant {
    public: 
        ForwardTrap();
        std::string getName();
        int add_cam_stack(std::vector<int>);
};

#endif
