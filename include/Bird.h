#ifndef __BIRD_H_
#define __BIRD_H_

#include <cstdlib>
#include <cmath>
#include "Angle.h"
//This is an entity that the environment is made of. The state of the environemnt, IN OUR CASE, is a collection
//of these entities

enum class Species{pursuer, evader};

class Bird{

    private:
        double _x{(double)rand()/RAND_MAX*10};
        double _y{(double)rand()/RAND_MAX*10};
        Angle _alpha{0};
        Species _species;

    public:

        Bird(double x, double y, double alpha);
        Bird(double x, double y, double alpha, Species spec);
        double get_x() const noexcept { return _x; }
        double get_y() const noexcept { return _y; }
        Angle get_alpha() const noexcept { return _alpha; }
        Species get_species() const noexcept {return _species; }

        void update(double velocity, Angle angle);
        void reset(double x, double y, Angle alpha);
        void reset(double r, double theta);
        void set_species(Species spec) {_species = spec;}

        static Bird random_bird(double r, double theta);
};

double relative_distance_squared(const Bird &a, const Bird &b);
Angle relative_angle(const Bird &a, const Bird &b, const double sin_alpha, const double cos_alpha);                


#endif