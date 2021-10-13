#include "Policy.h"

Action Policy::decide(Observable &o){

    if (this->_is_deterministic){
        return f(o);
    }
}