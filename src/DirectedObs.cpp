#include "DirectedObs.h"

// Direction angle_to_dir(double angle){
// }

DirectedObs::DirectedObs():
    UndirectedObs(),
    _dir{Direction::in}
    {}

DirectedObs::DirectedObs(Bird_in_scope b, Direction d):
    UndirectedObs(b),
    _dir{d}
    {}

std::size_t DirectedObs::to_int(){
    return static_cast<int>(_dir)*UndirectedObs::size + static_cast<int>(_b);
}