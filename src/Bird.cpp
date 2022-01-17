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

void Bird::update(const double velocity, const Angle angle, const double pbc){
        
    this->_alpha = (this->_alpha + angle);
    this->_x = this->_x + velocity*Angle::cos(this->_alpha);
    this->_y = this->_y + velocity*Angle::sin(this->_alpha);
    
    if(this->_x < -pbc)
        this->_x += 2*pbc;
    else if(this->_x > pbc)
        this->_x -= 2*pbc;
    
    if(this->_y < -pbc)
        this->_y += 2*pbc;
    else if(this->_y > pbc)
        this->_y -= 2*pbc;
}

void Bird::reset(double x, double y, Angle alpha){

    this->_x = x;
    this->_y = y;
    this->_alpha = alpha;
}

void Bird::reset(double r, double theta){

    this->_x = ((double) rand()/RAND_MAX + r)*cos(theta);
    this->_y = ((double) rand()/RAND_MAX + r)*sin(theta);
    this->_alpha = Angle(Rand::theta_rand());
}

Bird Bird::random_bird(double r, double theta){

    //double theta = (double) rand()/RAND_MAX * 2 * M_PI; //Random theta in polar coordinates
    double x = ((double) rand()/RAND_MAX + r)*cos(theta);
    double y = ((double) rand()/RAND_MAX + r)*sin(theta);
    double alpha = Rand::theta_rand();

    return Bird(x, y, alpha, Species::evader);
}

Angle relative_angle(const Bird &a, const Bird &b, const double sin_alpha, const double cos_alpha, const double pbc){

    double x;
    double y;

    std::tie(x,y) = relative_coordinate(a,b,pbc);

    double x_prime = cos_alpha*x - sin_alpha*y;
    double y_prime = sin_alpha*x + cos_alpha*y;

    return atan2(y_prime, x_prime);
}

double relative_distance_squared(const Bird &a, const Bird &b, const double pbc){

    //17-1: Update for periodic boundary conditions consistency
    // auto x = b.get_x() - a.get_x();
    // if(abs(x) > pbc)
    //     x += 2*pbc;

    // auto y = b.get_y() - a.get_y();
    // if(abs(y) > pbc)
    //     y += 2*pbc;
    double x;
    double y;

    std::tie(x,y) = relative_coordinate(a,b,pbc);

    return x*x + y*y;
}

std::pair<double,double> relative_coordinate(const Bird &a, const Bird &b, const double pbc){
    auto x = b.get_x() - a.get_x();

    //If b is on the right side, project left and viceversa
    if(x > pbc)
        x -= 2*pbc;
    else if(x < -pbc)
        x += 2*pbc;

    auto y = b.get_y() - a.get_y();
    if(y > pbc)
        y -= 2*pbc;
    else if(y < -pbc)
        y += 2*pbc;

    return std::make_pair(x,y);
}
