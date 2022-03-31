#ifndef __BOLTZMANN_H_
#define __BOLTZMANN_H_

#include "Observable.h"
#include "Action.h"
#include <list>

class Boltzmann{

    private:
        std::vector<std::vector<double>> _params;
        std::size_t _rows;
        std::size_t _cols;

    public:
        Boltzmann() = default;
        Boltzmann(const Boltzmann &p);
        Boltzmann(const std::size_t r, const std::size_t c);
        Boltzmann(Boltzmann &&b);
        
        //Getter
        std::vector<std::vector<double>> get_params() const {return _params;}
        double get(const std::size_t row, const std::size_t col) const {return _params[row][col];}
        std::vector<double> get_row(const std::size_t row) const {return _params[row];}

        const std::size_t get_rows() const {return _rows;}
        const std::size_t get_cols() const {return _cols;}

        template <typename T>
        Action decide(Observable<T> &o);

        template <typename T>
        void update(double coeffs, Observable<T> &o, Action &a);

        static Action sample_discrete(std::vector<double> p);

};

template <typename T>
Action Boltzmann::decide(Observable<T> &o){

    std::size_t row = o.index();
    auto actions_pars = get_row(row);

    std::vector<double> probabilities(this->_cols);
    double mean=0;
    double den=0;

    for(auto a:actions_pars){
        mean+=a;
    }

    mean /= this->_cols;

    for(std::size_t i=0; i<this->_cols; ++i){
        probabilities[i] = actions_pars[i] - mean;
        probabilities[i] = exp(probabilities[i]);
        den += probabilities[i];
    }

    for(std::size_t i=0; i<this->_cols; i++){
        probabilities[i] /= den;
    }

    return sample_discrete(probabilities);
}

//Non-linear approx version
template <typename T>
void Boltzmann::update(double coeffs, Observable<T> &o, Action &a){

    std::size_t row = o.index();    
    std::size_t col = static_cast<std::size_t>(a);
    
    std::vector<double> probabilities(3,0);
    double mean=0;

    for(std::size_t i=0; i<3; i++){
        mean+=get(row,i);
    }
    mean /= 3;

    double normalization = 0;
    for(std::size_t c=0; c<3; c++){
        probabilities[c] = get(row,c) - mean;
        probabilities[c] = exp(probabilities[c]);
        normalization += probabilities[c];
    }
    
    double policy = probabilities[col]/normalization;

    //Euclidean gradient
    // for(std::size_t a=0; a<3; a++){
    //     if(a==col){
    //         _params(row, col) += coeffs*(1 - policy_row_col);
    //     } else {
    //         _params(row, a) += coeffs*policy_row_col;
    //     }
    // }

    //Natural gradient
    _params[row][col] = _params[row][col] + coeffs/(policy+0.001);

}

#endif