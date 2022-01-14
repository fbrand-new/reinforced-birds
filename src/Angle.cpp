#include "Angle.h"

Angle::Angle():
    _a{0}
    {}

Angle::Angle(double a):
    _a{a}
    {
        //assert(_a >= -M_PI && _a <= M_PI);
    }

Angle Angle::operator+(const Angle &b){

    auto a = Angle(b.get()+this->get());
    a.to_range();
    return a;
}

Angle Angle::operator-(const Angle &b){
    
    auto a = Angle(b.get()-this->get());
    a.to_range();
    return a;
}

Angle& Angle::operator=(const Angle &b){

    _a = b.get();
    return *this;
}

void Angle::to_range(){
    if(_a <= -M_PI)
        _a += 2*M_PI;
    else if(_a > M_PI)
        _a -= 2*M_PI;
}

void Angle::to_range(Angle &a){
    if(a.get() <= -M_PI)
        a.set(a.get() + 2*M_PI);
    else if(a.get() > M_PI)
        a.set(a.get() - 2*M_PI);
}

double Angle::cos(const Angle &a){
    // if(a.get()<0)
    //     return std::cos(2*M_PI + a.get()); //On the negative y semiaxis
    // else
    //     return std::cos(a.get());
    //FIX 14-1. We already are at the standard orientation.
    //The std cosine is capable of computing a negative value cosine
    return std::cos(a.get());
}

double Angle::sin(const Angle &a){
    // if(a.get()<0)
    //     return std::sin(2*M_PI + a.get()); //On the negative y semiaxis
    // else
    //     return std::sin(a.get());
    //FIX 14-1. We already are at the standard orientation.
    //The std cosine is capable of computing a negative value sine
    return std::sin(a.get());
}

Angle Angle::abs(const Angle &a){
    return Angle(std::abs(a.get()));
}

std::ostream& operator<<(std::ostream & s, const Angle &a){
    s << a.get();
    return s;
}

bool operator<(const Angle &a, const Angle &b){
    return (a.get() < b.get());
}

bool operator>(const Angle &a, const Angle &b){
    return (a.get() > b.get());
}

// double Angle::to_0_2pi(double alpha){

//     //Takes alpha in the range 0_2pi IFF alpha in (-2pi, 4pi). 
//     //Range not checked to increase performances, careful

//     if(alpha < 0)
//         return alpha + 2*M_PI;
//     else if(alpha > 2*M_PI)
//         return alpha - 2*M_PI;
//     else
//         return alpha;
// }