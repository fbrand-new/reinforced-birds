#include "UndirectedObs.h"

std::size_t UndirectedObs::size{4};

UndirectedObs::UndirectedObs():
    _b{Bird_in_scope::none}
    {}

UndirectedObs::UndirectedObs(Bird_in_scope b):
    _b{b}
    {}

// UndirectedObs& UndirectedObs::operator=(const UndirectedObs& u){

//     auto b = u.get_bird_in_scope();
//     return UndirectedObs(b);

// }

bool UndirectedObs::operator==(const UndirectedObs &u){

    if (this->get_bird_in_scope() == u.get_bird_in_scope())
        return true;
    else
        return false;
}

std::size_t UndirectedObs::to_int(){
    return static_cast<int>(_b);
}