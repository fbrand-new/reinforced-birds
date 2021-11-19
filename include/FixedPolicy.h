#ifndef __FIXEDPOLICY_H__
#define __FIXEDPOLICY_H__

#include "Policy.h"

class FixedPolicy : public Policy{

    private:
        FixedPolicy(const Policy &p);
        FixedPolicy(const std::size_t rows, const std::size_t cols);

    public:
        Action decide(Observable &o) override;
        void update(double coeffs, Observable &o, Action &a) override;

};

#endif