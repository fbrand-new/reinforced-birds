#include "Bird.h"

void Bird::update(double velocity, double angle){
        
    this->_alpha = this->_alpha + angle;
    this->_x = this->_x + velocity*cos(this->_alpha);
    this->_y = this->_y + velocity*sin(this->_alpha);
}

void Bird::reset(){

    this->_x = (double)rand()/RAND_MAX;
    this->_y = (double)rand()/RAND_MAX;
    this->_alpha = 0;
}