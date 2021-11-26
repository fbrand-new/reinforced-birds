#ifndef __DIRECTEDOBS_H__
#define __DIRECTEDOBS_H__

#include "UndirectedObs.h"

enum class Direction{in,out};
//Direction angle_to_dir(double angle);

class DirectedObs : public UndirectedObs {

    private:
        Direction _dir;

    public:
        DirectedObs();
        DirectedObs(Bird_in_scope b, Direction d);
        std::size_t to_int();
};

#endif