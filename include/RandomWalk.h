#ifndef __RANDOMWALK_H_
#define __RANDOMWALK_H_

#include <cstdlib>
#include "Action.h"
#include "Observable.h"

class RandomWalk{

    public:
        //Default ctor
        RandomWalk() = default;

        template <typename T>
        Action decide(Observable<T> &o);

};

template <typename T>
Action RandomWalk::decide(Observable<T> &o){
    return static_cast<Action>( rand() % (static_cast<int>(Action::right)+1));
}


#endif