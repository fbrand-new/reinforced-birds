#include "RandomWalk.h"

RandomWalk::RandomWalk(const Policy &p): Policy(p)
    {}

RandomWalk::RandomWalk(const std::size_t rows, const std::size_t cols) : Policy(rows, cols)
    {}

Action RandomWalk::decide(Observable &o){
    return static_cast<Action>( rand() % (static_cast<int>(Action::right)+1));
}