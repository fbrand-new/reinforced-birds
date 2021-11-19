#include "Boltzmann.h"
#include <iostream>

Boltzmann::Boltzmann(const Policy &p) : Policy(p)
    {}

Boltzmann::Boltzmann(const std::size_t rows, const std::size_t cols): Policy(rows, cols)
    {}

Action Boltzmann::decide(Observable &o){

    std::size_t row = o.index();
    auto actions_pars = get_row(row);
    std::size_t cols = actions_pars.cols();

    std::vector<double> probabilities(cols);
    double mean=0;
    double den=0;

    for(std::size_t i=0; i<cols; i++){
        mean+=actions_pars(0,i);
    }

    mean /= cols;

    for(std::size_t i=0; i<cols; ++i){
        probabilities[i] = actions_pars(0,i) - mean;
        den+=exp(probabilities[i]);
    }
    
    for(std::size_t i=0; i<cols; i++){
        probabilities[i] = exp(probabilities[i])/den;
    }

    return sample_discrete(probabilities);

}

void Boltzmann::update(double coeffs, Observable &o, Action &a){

    std::size_t row = o.index();
    std::size_t col = static_cast<std::size_t>(a);
    
    std::vector<double> probabilities(3);
    double mean=0;

    for(std::size_t i=0; i<3; i++){
        mean+=_params(row,i);
    }
    mean /= 3;

    double normalization = 0;
    for(std::size_t c=0; c<3; c++){
        probabilities[c] = _params(row,c) - mean;
        normalization += exp(probabilities[c]);
    }
    
    double policy_row_col = exp(probabilities[col])/normalization;

    //Euclidean gradient
    // for(std::size_t a=0; a<3; a++){
    //     if(a==col){
    //         _params(row, col) += coeffs*(1 - policy_row_col);
    //     } else {
    //         _params(row, a) += coeffs*policy_row_col;
    //     }
    // }

    //Natural gradient
    _params(row, col) += coeffs/(policy_row_col+0.001);

}

Action Boltzmann::sample_discrete(std::vector<double> p){

    double r = (double)rand()/RAND_MAX;
    
    //TODO: we could be more elegant here
    if(r<p[0])
        return Action::left;
    else if(r<(p[0]+p[1]))
        return Action::straight;
    else    
        return Action::right;

}