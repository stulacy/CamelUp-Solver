#include "forwardtrap.h"

ForwardTrap::ForwardTrap() {};

std::string ForwardTrap::getName() {
    return "Forward trap";
}
        
int ForwardTrap::add_cam_stack(std::vector<int> camels) {
    Rcpp::Rcout << "Hit forward trap\n";
    return 1;
}
