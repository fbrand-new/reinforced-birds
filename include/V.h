#ifndef __V_H_
#define __V_H_

#include "Observable.h"

class V{

    private:
        std::vector<double> _w;
        //Eigen::VectorXd _w;

    public:
        V(std::size_t size);
        double& operator[](Observable &o);
        double& operator[](std::size_t i);
};

#endif