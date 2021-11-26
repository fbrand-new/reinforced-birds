#ifndef __CHASE_H_
#define __CHASE_H_

#include "Observable.h"
#include "Action.h"

class Chase{

    public:
        Chase() = default;

        template <typename T>
        Action decide(Observable<T> &o);
};

template <typename T>
Action Chase::decide(Observable<T> &o){

    std::size_t sectors_num = o.get_sectors_num();
    std::size_t straight = sectors_num/2;

    for(std::size_t i=0; i<sectors_num; ++i){
        if(o.get_sector(i)==Bird_in_scope::foe){
            if(i<straight)
                return Action::right;
            else if(i==straight)
                return Action::straight;
            else
                return Action::left;
        }
    }

    return randomAction();
}

#endif