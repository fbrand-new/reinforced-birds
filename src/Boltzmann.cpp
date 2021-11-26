#include "Boltzmann.h"
#include <iostream>

Boltzmann::Boltzmann(const Boltzmann &p):
    _params{p.get_params()}
    {}

Boltzmann::Boltzmann(const std::size_t r, const std::size_t c):
    _params(r, std::vector<double>(c,0.1)),
    _rows{r},
    _cols{c}
    {}

Boltzmann::Boltzmann(Boltzmann &&b):
    _params{std::move(b.get_params())},
    _rows{std::move(b.get_rows())},
    _cols{std::move(b.get_cols())}
    {}

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