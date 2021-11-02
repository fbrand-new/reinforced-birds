#include "State.h"

State::State(unsigned int i):
    //_birds(i, Bird(i==0 ? 2*rand()/RAND_MAX + 1 : 1, i==0 ? 3*rand()/RAND_MAX + 5 : 5, (rand()%12 -6)*M_PI/6))
    _birds(i,Bird((double)rand()/RAND_MAX+3, (double)rand()/RAND_MAX+4, (rand()%12 -6)*M_PI/6))
    {
        _birds[0].reset(0,0,(rand()%12 -6)*M_PI/6);
    }

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

    _birds[0].reset(0,0,(rand()%12 -6)*M_PI/6);
    for(std::size_t i=1; i< _birds.size(); ++i){
        //_birds[i].reset(2*i + 1, 3*i + 5, (rand()%12 -6)*M_PI/6);
        _birds[i].reset((double)rand()/RAND_MAX+3, (double)rand()/RAND_MAX+4, (rand()%12 -6)*M_PI/6);
    }
}
