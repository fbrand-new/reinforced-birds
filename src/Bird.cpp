#include "Bird.h"

void Bird::update(double velocity, double angle){
        
    this->_alpha = this->_alpha + angle;
    if(this->_alpha < 0)
        this->_alpha += 2*M_PI;
    else if(this->_alpha >= 2*M_PI)
        this->_alpha -= 2*M_PI;
    this->_x = this->_x + velocity*cos(this->_alpha);
    this->_y = this->_y + velocity*sin(this->_alpha);
}

Bird::Bird(double x, double y, double alpha):
    _x{x},
    _y{y},
    _alpha{alpha}
    {}

void Bird::reset(double x, double y, double alpha){

    this->_x = x;
    this->_y = y;
    this->_alpha = alpha;
}