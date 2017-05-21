#ifndef OCCUPANT_H 
#define OCCUPANT_H 
#include <iostream>
#include <vector>
#include <Rcpp.h>

class TileOccupant {
    public: 
        TileOccupant(void);
        virtual std::string getName() = 0;
        virtual int add_cam_stack(std::vector<int>, bool) = 0;
};

#endif
