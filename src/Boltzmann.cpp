#include "Boltzmann.h"

Boltzmann::Boltzmann(const Policy &p) : Policy(p)
    {}

Boltzmann::Boltzmann(const std::size_t rows, const std::size_t cols): Policy(rows, cols)
    {}

Action Boltzmann::decide(Observable &o){

    std::size_t row = o.index();
    auto actions_pars = get_row(row);

    auto den = 0;
    for(std::size_t i=0; i<actions_pars.cols(); i++){
        den+=exp(actions_pars(0,i));
    }

    std::vector<double> probabilities(actions_pars.cols());
    for(std::size_t i=0; i<actions_pars.cols(); i++){
        probabilities[i] = exp(actions_pars(0,i))/den;
    }

    return sample_discrete(probabilities);

}

void Boltzmann::update(double coeffs, Observable &o, Action &a){

    std::size_t row = o.index();
    std::size_t col = static_cast<std::size_t>(a);
    
    double normalization = 0;
    for(std::size_t c=0; c<3; c++)
        normalization += exp(_params(row,c));

    double old_par = _params(row, col);
    _params(row, col) += coeffs*(1 - exp(old_par)/normalization);
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