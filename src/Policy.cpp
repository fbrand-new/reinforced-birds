#include "Policy.h"
#include <iostream>
#include <random>

//This should initialize a random matrix with coefficients \in [0,1]
//Let's initialize instead a truly random policy
Policy::Policy(size_t rows, size_t cols):
    _params{Eigen::MatrixXd(rows, cols)}
    {
        //We initialize a stochastic matrix
        for(std::size_t i=0; i<rows; i++){
            for(std::size_t j=0; j<cols; j++){
                double l = 0.1;
                _params(i,j) = l;
            }
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

Action Policy::randomAction(){

    std::random_device rd;
    std::mt19937 mt(rd);
    std::uniform_int_distribution<int> unif(0,2);

    return static_cast<Action>(unif(mt));

}