#ifndef __ANGLE_H__
#define __ANGLE_H__

#include <math.h>
#include <assert.h>
#include <iostream>

/*
    This class defines an angular coordinate between (-PI,PI]
*/


class Angle{

    //The cut is fixed at -M_PI!

    private:
        double _a;
        //double _axis; //Identifies the cut axis: we compute the angle starting from _axis up to a full rotation
        void to_range();
        static void to_range(Angle &a);

    public:
        Angle();
        Angle(double a);
        Angle operator+(const Angle &b);
        Angle operator-(const Angle &b);
        Angle& operator=(const Angle &b);

        //Getters
        const double get() const {return _a;}
        //const double get_axis() const {return _axis;}

        //Setters
        void set(double a) {_a = a;}

        //Trigonometry
        static double cos(const Angle &a);
        static double sin(const Angle &a);

        //Takes alpha in the range 0_2pi IFF alpha in (-2pi, 4pi). 
        //Range not checked to increase performances, careful
        //static double to_0_2pi(double alpha); 
        //static double to_
};

std::ostream& operator<<(std::ostream & s, const Angle &a);

#endif