#ifndef __V_H_
#define __V_H_

#include "Observable.h"

class V{

    private:
        std::vector<double> _w;
        //Eigen::VectorXd _w;

    public:
        V(std::size_t size);

        template <typename T>
        double& operator[](Observable<T> &o);
        double& operator[](std::size_t i);
};

template <typename T>
double& V::operator[](Observable<T> &o){
    return _w[o.index()];
}

#endif