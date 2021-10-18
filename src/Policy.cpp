#include "Policy.h"
#include <iostream>

//This should initialize a random matrix with coefficients \in [0,1]
Policy::Policy(size_t rows, size_t cols):
    _params{Eigen::MatrixXd(rows, cols)}
    {
        //We initialize a stochastic matrix
        for(std::size_t i=0; i<rows; i++){
            double remainder=1;
            for(std::size_t j=0; j<cols-1; j++){
                double l = (double)rand()/RAND_MAX;
                _params(i,j) = l;
                remainder -= l;
            }
            _params(i,cols-1) = remainder;
        }
    }

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

double Policy::get(const std::size_t row, const std::size_t col){
    return _params(row, col);
}

Eigen::MatrixXd Policy::get_row(const std::size_t row){
    return _params.row(row);
}

Action Policy::decide(Observable &o){

    std::cout << "Not so random policy" << std::endl;
    return Action::straight;
}

void Policy::update(double coeffs, Observable &o, Action &a){
}