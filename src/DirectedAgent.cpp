#include "DirectedAgent.h"

//Some std::pair utilities for the rest of the code
int_pair operator+(const int_pair &a, const int_pair &b){
    return std::make_pair(a.first+b.first, a.second+b.second);
}

std::size_t inner_sum(const int_pair &a){
    return a.first+a.second;
}

template<>
void DirectedAgent<Boltzmann>::update_policy(double coeff, Observable<DirectedObs> &o, Action &a){
    _p->update(coeff, o, a);
}