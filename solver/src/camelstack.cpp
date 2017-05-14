#include "camelstack.h"

CamelStack::CamelStack(std::vector<int> camels) : camels(camels) {}

std::string CamelStack::getName() {
    return "Camel stack";
}

int CamelStack::add_cam_stack(std::vector<int> new_cams, bool reverse) {
    Rcpp::Rcout << "Adding camels to existing camels: ";
    for (auto i : camels) {
        Rcpp::Rcout << i << ",";
    }
    Rcpp::Rcout << "\n";
    
    if (reverse) {
        camels.insert( camels.begin(), new_cams.begin(), new_cams.end() );
    } else {
        camels.insert( camels.end(), new_cams.begin(), new_cams.end() );
    }
    Rcpp::Rcout << "Just added new camels to camel stack, which now looks like: ";
    for (auto i : camels) {
        Rcpp::Rcout << i << ",";
    }
    Rcpp::Rcout << "\n";
    return 0;
}
