#ifndef __BIRD_H_
#define __BIRD_H_

#include <cstdlib>
//This is an entity that the environment is made of. The state of the environemnt, IN OUR CASE, is a collection
//of these entities
class Bird{

    private:
        double _x{(double)rand()/RAND_MAX};
        double _y{(double)rand()/RAND_MAX};
        double _alpha{0};

    public:
        double get_x() const noexcept { return _x; }
        double get_y() const noexcept { return _y; }
        double get_alpha() const noexcept { return _alpha; }

        void update(double velocity, double angle);

};

#endif