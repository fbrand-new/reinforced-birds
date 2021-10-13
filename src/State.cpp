#include "State.h"

State::State(unsigned int i):
    _birds(i)
    {}

void State::update(double velocity, double angle){
    this->_alpha = this->_alpha + angle;
    this->_x = this->_x + velocity*cos(this->_alpha);
    this->_y = this->_y + velocity*sin(this->_alpha);
}

std::ofstream& operator <<(std::ofstream & os, State &s){

    auto birds = s.get_birds();

    for(std::size_t i; i< birds.size(); ++i){
        os << birds[i].get_x() << "," << birds[i].get_y() << std::endl;
    }
    
    return os;
}

std::ostream& operator <<(std::ostream & os, State &s){
        
    auto birds = s.get_birds();

    for(std::size_t i; i< birds.size(); ++i){
        os << birds[i].get_x() << "," << birds[i].get_y() << std::endl;
    }
    
    return os;
}

