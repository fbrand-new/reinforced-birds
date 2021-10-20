#include "State.h"

State::State(unsigned int i):
    _birds(i, Bird(2*i + 1, 3*i + 5, (rand()%12 -6)*M_PI/6))
    {}

void State::update(double velocity, double angle, unsigned int i){

    //bird_i = _birds[i];
    _birds[i].update(velocity, angle);

}

std::ofstream& operator <<(std::ofstream & os, State &s){

    auto birds = s.get_birds();

    for(std::size_t i=0; i< birds.size(); ++i){
        os << birds[i].get_x() << "," << birds[i].get_y();
        if(i<birds.size()-1) 
            os << ",";
    }
    
    os << std::endl;
    return os;
}

std::ostream& operator <<(std::ostream & os, State &s){
        
    auto birds = s.get_birds();

    for(std::size_t i=0; i< birds.size(); ++i){
        os << birds[i].get_x() << "," << birds[i].get_y();
        if(i<birds.size()-1) 
            os << ",";
    }
    
    os << std::endl;
    return os;
}

void State::reset(){

    for(std::size_t i=0; i< _birds.size(); ++i){
        _birds[i].reset(2*i + 1, 3*i + 5, (rand()%12 -6)*M_PI/6);
    }
}
