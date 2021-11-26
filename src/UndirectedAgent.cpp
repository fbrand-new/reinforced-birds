#include "UndirectedAgent.h"

template<>
void UndirectedAgent<Boltzmann>::update_policy(double coeff, Observable<UndirectedObs> &o, Action &a){
    _p->update(coeff, o, a);
}