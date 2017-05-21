#include "camelstack.h"

CamelStack::CamelStack(std::vector<int> camels) : camels(camels) {}

std::string CamelStack::getName() {
    return "Camel stack";
}

int CamelStack::add_cam_stack(std::vector<int> new_cams, bool reverse) {
    if (reverse) {
        camels.insert( camels.begin(), new_cams.begin(), new_cams.end() );
    } else {
        camels.insert( camels.end(), new_cams.begin(), new_cams.end() );
    }
    return 0;
}
