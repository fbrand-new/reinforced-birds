#ifndef __BIRD_H_
#define __BIRD_H_

#include <cstdlib>
#include <cmath>
//This is an entity that the environment is made of. The state of the environemnt, IN OUR CASE, is a collection
//of these entities
class Bird{

    private:
        double _x{(double)rand()/RAND_MAX*10};
        double _y{(double)rand()/RAND_MAX*10};
        double _alpha{0};

    public:

        Bird(double x, double y, double alpha);
        double get_x() const noexcept { return _x; }
        double get_y() const noexcept { return _y; }
        double get_alpha() const noexcept { return _alpha; }

        void update(double velocity, double angle);
        void reset(double x, double y, double alpha);

};

#endif