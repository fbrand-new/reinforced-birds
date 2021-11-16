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

void Bird::update(double velocity, double angle){
        
    this->_alpha = this->_alpha + angle;
    if(this->_alpha < 0)
        this->_alpha += 2*M_PI;
    else if(this->_alpha >= 2*M_PI)
        this->_alpha -= 2*M_PI;
    this->_x = this->_x + velocity*cos(this->_alpha);
    this->_y = this->_y + velocity*sin(this->_alpha);
}

void Bird::reset(double x, double y, double alpha){

    this->_x = x;
    this->_y = y;
    this->_alpha = alpha;
}

void Bird::reset(double r, double theta){

    this->_x = ((double) rand()/RAND_MAX + r)*cos(theta);
    this->_y = ((double) rand()/RAND_MAX + r)*sin(theta);
    this->_alpha = (rand()%12)*M_PI/6;
}

Bird Bird::random_bird(double r, double theta){

    //double theta = (double) rand()/RAND_MAX * 2 * M_PI; //Random theta in polar coordinates
    double x = ((double) rand()/RAND_MAX + r)*cos(theta);
    double y = ((double) rand()/RAND_MAX + r)*sin(theta);
    double alpha = (rand()%12)*M_PI/6;

    return Bird(x, y, alpha, Species::evader);
}