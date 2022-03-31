#ifndef __STATE_H_
#define __STATE_H_

#include "Bird.h"
#include <cstdlib>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "Rand.h"

class State{

    private:
        std::vector<Bird> _birds;
        std::size_t _pursuers_num;
        double _pbc; //Periodic boundary conditions are the same for x and y
        double _av_dist;
        
    public:

        //Ctors
        //State(std::size_t i);
        State(std::size_t i, double pbc, double av_dist);

        //Getters
        auto & get_birds() const {return _birds;}
        std::size_t get_pursuer_num() const {return _pursuers_num;}
        auto & get_pbc() const {return _pbc;}

        auto size() const {return _birds.size();}
        void update(double velocity, double angle, unsigned int i);
        void reset();

        //Utility function
        static double rand_a();
};

std::ofstream& operator <<(std::ofstream & os, State &s);
std::ostream& operator <<(std::ostream & os, State &s);



#endif