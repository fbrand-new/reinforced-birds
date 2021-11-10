#ifndef __OBSERVABLE__
#define __OBSERVABLE__

#include <vector>
#include <math.h>
#include <fstream>

//enum class observed{none, pursuer, evader, both};

class Observable{

    private:
        //It's better to have 0 in a sector if no one is there, 1 if there are preys
        //2 if there is even a single predator
        std::vector<unsigned int> _o;
        std::size_t _sectors;
        std::size_t _max_val;

    public:
        Observable();
        Observable(std::size_t sectors);
        Observable(std::size_t sectors, std::size_t max_val);

        std::vector<unsigned int> get_obs();
        std::size_t get_sectors_num();
        std::size_t get_max_val();
        std::size_t index();

        void empty_sector(unsigned int i);
        void non_empty_sector(unsigned int i);
        //void non_empty_sector(unsigned int i, bool pred);
        void non_empty_sector(unsigned int i, unsigned int s);
};

std::ostream& operator <<(std::ostream & os, Observable &o);

#endif