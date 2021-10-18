#ifndef __BOLTZMANN_H_
#define __BOLTZMANN_H_

#include "Policy.h"
#include "Observable.h"

class Boltzmann : public Policy{

    public:
        Boltzmann(const Policy &p);
        Boltzmann(const std::size_t rows, const std::size_t cols);
        Action decide(Observable &o) override;
        void update(double coeffs, Observable &o, Action &a) override;

        static Action sample_discrete(std::vector<double> p);

};

#endif