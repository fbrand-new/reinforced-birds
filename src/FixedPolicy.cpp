#include "FixedPolicy.h"

FixedPolicy::FixedPolicy(const Policy &p) : Policy(p)
    {}

FixedPolicy::FixedPolicy(const std::size_t rows, const std::size_t cols) : Policy(rows,cols)
    {}

Action FixedPolicy::decide(Observable &o){

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