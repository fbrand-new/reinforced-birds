#include "Bird.h"

void Bird::update(double velocity, double angle){
        
    this->_alpha = this->_alpha + angle;
    this->_x = this->_x + velocity*cos(this->_alpha);
    this->_y = this->_y + velocity*sin(this->_alpha);
}