#ifndef __STATE_H_
#define __STATE_H_

#include "Bird.h"
#include <cstdlib>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>

class State{

    private:
        std::vector<Bird> _birds;

    public:
        State(unsigned int i);
        auto & get_birds() const {return _birds;}
        auto size() const {return _birds.size();}
        void update(double velocity, double angle, unsigned int i);
};

std::ofstream& operator <<(std::ofstream & os, State &s);
std::ostream& operator <<(std::ostream & os, State &s);

#endif