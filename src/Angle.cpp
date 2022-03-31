#include "Angle.h"

Angle::Angle():
    _a{0}
    {}

Angle::Angle(double a):
    _a{a}
    {}

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
    return std::cos(a.get());
}

double Angle::sin(const Angle &a){
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