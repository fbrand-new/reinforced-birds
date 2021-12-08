#ifndef __DIRECTEDOBS_H__
#define __DIRECTEDOBS_H__

#include "UndirectedObs.h"

//This multiplies the states in each sector by 2
enum class Direction{in,out};

class DirectedObs : public UndirectedObs {

    private:
        Direction _dir;

    public:
        DirectedObs();
        DirectedObs(Bird_in_scope b, Direction d);
        std::size_t to_int();
};

#endif