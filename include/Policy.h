#ifndef __POLICY_H_
#define __POLICY_H_

#include "Action.h"
#include "Observable.h"
#include <cstdlib>

class Policy{

    protected:
        std::vector<std::vector<double>> _params;
        //Eigen::MatrixXd _params;

    public:
        std::size_t rows;
        std::size_t cols;

        //Constructors
        Policy() = default;
        Policy(size_t r, size_t c);
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
        //Eigen::MatrixXd get_params() const {return _params;}
        std::vector<std::vector<double>> get_params() const {return _params;}
        double get(const std::size_t row, const std::size_t col);
        std::vector<double> get_row(const std::size_t row);
        //Eigen::MatrixXd get_row(const std::size_t row);

        //Setter
        //void set_params(Eigen::MatrixXd pars) {_params = pars;}
        void set_params(std::vector<std::vector<double>> pars);
        void set_param(std::size_t row, std::size_t col, double par); //Sets param to par
        void update_param(std::size_t row, std::size_t col, double par); //Adds par to param
        
        virtual Action decide(Observable &o);
        virtual void update(double coeffs, Observable &o, Action &a);

        static Action randomAction();
};

#endif