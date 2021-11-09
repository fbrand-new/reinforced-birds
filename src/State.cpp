#include "State.h"

State::State(unsigned int i):
    _birds(i, Bird::random_bird(10,0))
    //_birds(i,Bird(rand_x(), rand_y(), rand_a(),Species::evader))
    {
        _birds[0].reset(0,0,rand_a());
        _birds[0].set_species(Species::pursuer);
    }

void State::update(double velocity, double angle, unsigned int i){

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

    _birds[0].reset(0,0,rand_a());
    double theta = (double) rand()/RAND_MAX * 2 * M_PI; //Random theta in polar coordinates
    for(std::size_t i=1; i< _birds.size(); ++i){
        //_birds[i].reset(rand_x(), rand_y(), rand_a());
        _birds[i].reset(10, theta);
    }
}

double State::rand_x(){
    return (double)rand()/RAND_MAX+10;
}

double State::rand_y(){
    return (double)rand()/RAND_MAX+10;
}

double State::rand_a(){
    return (rand()%12 -6)*M_PI/6;
}

