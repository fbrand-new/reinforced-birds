#ifndef __RANDOMWALK_H_
#define __RANDOMWALK_H_

#include "Policy.h"
#include <cstdlib>

class RandomWalk : public Policy{

    public:
        //Default ctor
        RandomWalk(const Policy &p);
        RandomWalk(const std::size_t rows, const std::size_t cols);
        Action decide(Observable &o) override;
};

#endif