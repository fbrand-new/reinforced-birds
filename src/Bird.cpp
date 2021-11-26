#include "Bird.h"

Bird::Bird(double x, double y, double alpha):
    _x{x},
    _y{y},
    _alpha{alpha}
    {}

Bird::Bird(double x, double y, double alpha, Species spec):
    _x{x},
    _y{y},
    _alpha{alpha},
    _species{spec}
    {}

void Bird::update(double velocity, Angle angle){
        
    this->_alpha = (this->_alpha + angle);
    this->_x = this->_x + velocity*Angle::cos(this->_alpha);
    this->_y = this->_y + velocity*Angle::sin(this->_alpha);
}

void Bird::reset(double x, double y, Angle alpha){

    this->_x = x;
    this->_y = y;
    this->_alpha = alpha;
}

void Bird::reset(double r, double theta){

    this->_x = ((double) rand()/RAND_MAX + r)*cos(theta);
    this->_y = ((double) rand()/RAND_MAX + r)*sin(theta);
    this->_alpha = Angle((rand()%12)*M_PI/6 - M_PI);
}

Bird Bird::random_bird(double r, double theta){

    //double theta = (double) rand()/RAND_MAX * 2 * M_PI; //Random theta in polar coordinates
    double x = ((double) rand()/RAND_MAX + r)*cos(theta);
    double y = ((double) rand()/RAND_MAX + r)*sin(theta);
    double alpha = (rand()%12)*M_PI/6 - M_PI;

    return Bird(x, y, alpha, Species::evader);
}

Angle relative_angle(const Bird &a, const Bird &b, const double sin_alpha, const double cos_alpha){

    double rel_x = b.get_x() - a.get_x();
    double rel_y = b.get_y() - a.get_y();

    double x_prime = cos_alpha*rel_x - sin_alpha*rel_y;
    double y_prime = sin_alpha*rel_x + cos_alpha*rel_y;

    return atan2(y_prime, x_prime);
}

double relative_distance_squared(const Bird &a, const Bird &b){

    auto x = a.get_x() - b.get_x();
    auto y = a.get_y() - b.get_y();
    return x*x + y*y;
    
}
