#include "State.h"

State::State(std::size_t i, double pbc, double av_dist):
    _birds(i, Bird::random_bird(av_dist,0)),
    _pursuers_num{1},
    _pbc{pbc},
    _av_dist{av_dist}
    {
        _birds[0].reset(0,0,rand_a());
        _birds[0].set_species(Species::pursuer);
    }

void State::update(double velocity, double angle, unsigned int i){

    _birds[i].update(velocity, angle, _pbc);

}

void State::reset(){

    _birds[0].reset(0,0,rand_a());
    double theta = Rand::theta_rand(); //Random theta in polar coordinates
    for(std::size_t i=1; i< _birds.size(); ++i){
        //_birds[i].reset(rand_x(), rand_y(), rand_a());
        _birds[i].reset(_av_dist, theta);
    }
}

std::ofstream& operator <<(std::ofstream & os, State &s){

    auto birds = s.get_birds();

    for(std::size_t i=0; i< birds.size(); ++i){
        os << birds[i].get_x() << "," << birds[i].get_y() << "," << birds[i].get_alpha();
        if(i<birds.size()-1) 
            os << ",";
    }
    
    os << std::endl;
    return os;
}

std::ostream& operator <<(std::ostream & os, State &s){
        
    auto birds = s.get_birds();

    for(std::size_t i=0; i< birds.size(); ++i){
        os << birds[i].get_x() << "," << birds[i].get_y() << "," << birds[i].get_alpha();
        if(i<birds.size()-1) 
            os << ",";
    }
    
    os << std::endl;
    return os;
}

// double State::rand_x(){
//     return Rand::unit_rand() + _av_dist;
// }

// double State::rand_y(){
//     return Rand::unit_rand() + 10;
// }

double State::rand_a(){
    return Rand::theta_rand();
}

