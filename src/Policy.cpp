#include "Policy.h"
#include <iostream>

Policy::Policy(size_t params_num):
    _params{std::vector<double>(params_num)}
    {}

//Copy semantics
Policy::Policy(const Policy &p):
    _params{p.get_params()}
    {}

Policy& Policy::operator=(const Policy &p){
    _params = p.get_params();
    return *this;
}

//Move semantics
Policy::Policy(Policy &&p):
    _params{std::move(p.get_params())}
    {}

Policy& Policy::operator=(Policy &&p){
    _params = std::move(p.get_params());
    return *this;
}

Action Policy::decide(Observable &o){

    std::cout << "Not so random policy" << std::endl;
    return Action::straight;
}