#ifndef __OBSERVABLE__
#define __OBSERVABLE__

#include <vector>
#include <math.h>
#include <fstream>

class Observable{

    private:
        std::vector<unsigned int> _o;
        std::size_t _sectors;
        std::size_t _max_val;

    public:
        Observable();
        Observable(size_t sectors);

        std::vector<unsigned int> get_obs();
        std::size_t get_sectors_num();
        std::size_t index();

        void empty_sector(unsigned int i);
        void non_empty_sector(unsigned int i);
};

std::ostream& operator <<(std::ostream & os, Observable &o);

#endif