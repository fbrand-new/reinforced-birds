#include "Tools.h"

template<class ... Types>
std::ofstream& Tools::build_header(std::ofstream &s, std::string k, Types ... args){

    if(k == "episode"){
        s  << "Episode,EndTime,PredatorTraining"  << std::endl;
    } else if(k == "policy"){
        s << "Left,Straight,Right" << std::endl;
    } else if(k == "trajectory"){
        for(std::size_t i=0; i<args[0]; ++i){
            s << "x" << i << ",y" << i;
            if(i<num_of_birds-1) 
                s << ",";
        }
        s << std::endl;
    }
    return s;
}

