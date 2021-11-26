#include "Action.h"

Action randomAction(){

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> unif(0,2);

    return static_cast<Action>(unif(mt));

}