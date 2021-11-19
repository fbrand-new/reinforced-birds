#ifndef __OBSERVABLE__
#define __OBSERVABLE__

#include <vector>
#include <math.h>
#include <fstream>

enum class Bird_in_scope{none, foe, brother, both};

class Observable{

    private:
        std::vector<Bird_in_scope> _o;
        std::size_t _sectors;
        std::size_t _max_val;

    public:
        Observable();
        Observable(std::size_t sectors);
        Observable(std::size_t sectors, std::size_t max_val);

        //Getter
        std::vector<Bird_in_scope> get_obs();
        std::size_t get_sectors_num();
        std::size_t get_max_val();
        Bird_in_scope get_sector(std::size_t i) { return _o[i];}
        

        //Setters
        void set_sector(std::size_t i, Bird_in_scope b);

        std::size_t index();
};

std::ostream& operator <<(std::ostream & os, Observable &o);
std::ofstream& operator <<(std::ofstream & os, Observable &o);

#endif