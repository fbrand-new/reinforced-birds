#ifndef __POLICY_H_
#define __POLICY_H_

#include "Action.h"
#include "Observable.h"
#include <Eigen/Dense>

class Policy{

    private:
        std::vector<double> _params;

    public:

        //Constructors
        Policy() = default;
        Policy(size_t params_num);
        Policy(const Policy &p);

        //Copy assignment
        Policy& operator=(const Policy &p);
        //Move ctor
        Policy(Policy &&p);        
        //Move assignment
        Policy& operator=(Policy &&p);

        //Destructors
        ~Policy(){};

        //Getter
        std::vector<double> get_params() const {return _params;}

        //Setter
        void set_params(std::vector<double> pars) {_params = pars;}
        
        virtual Action decide(Observable &o);
};

#endif