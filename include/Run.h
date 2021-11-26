#ifndef __RUN_H_
#define __RUN_H_

#include "Action.h"
#include "Observable.h"

class Run{

    public:
        Run() = default;

        template <typename T>
        Action decide(Observable<T> &o);
};

template <typename T>
Action Run::decide(Observable<T> &o){

    std::size_t sectors_num = o.get_sectors_num();
    std::size_t straight = sectors_num/2;

    for(std::size_t i=0; i<sectors_num; ++i){
        if(o.get_sector(i)==Bird_in_scope::foe){
            if(i<straight)
                return Action::left;
            else if(i==straight)
                return Action::straight;
            else
                return Action::right;
        }
    }

    return randomAction();      
}

#endif