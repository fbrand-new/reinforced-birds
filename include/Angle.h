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

        //Setters
        void set(double a) {_a = a;}

        //Trigonometry
        static double cos(const Angle &a);
        static double sin(const Angle &a);
        static Angle abs(const Angle &a);
};

std::ostream& operator<<(std::ostream & s, const Angle &a);
bool operator<(const Angle &a, const Angle &b);
bool operator>(const Angle &a, const Angle &b);

#endif