#include "backwardtrap.h"

BackwardTrap::BackwardTrap() {};

std::string BackwardTrap::getName() {
    return "Backward trap";
}

int BackwardTrap::add_cam_stack(std::vector<int> camels) {
    Rcpp::Rcout << "Hit backward trap\n";
    return -1;
}
