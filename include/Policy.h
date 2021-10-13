#ifndef __POLICY_H_
#define __POLICY_H_

#include "Action.h"
#include "Observable.h"

class Policy{

    private:

        
    public:
        Action decide(Observable &o);

};

#endif