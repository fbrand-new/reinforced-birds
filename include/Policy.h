#ifndef __POLICY_H_
#define __POLICY_H_

#include "Action.h"
#include "Observable.h"
#include <Eigen/Dense>
#include <cstdlib>

class Policy{

    protected:
        Eigen::MatrixXd _params;

    public:

        //Constructors
        Policy() = default;
        Policy(size_t rows, size_t cols);
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
        Eigen::MatrixXd get_params() const {return _params;}
        double get(const std::size_t row, const std::size_t col);
        Eigen::MatrixXd get_row(const std::size_t row);

        //Setter
        void set_params(Eigen::MatrixXd pars) {_params = pars;}
        
        virtual Action decide(Observable &o);
        virtual void update(double coeffs, Observable &o, Action &a);

        static Action randomAction();
};

#endif