#include <iostream>
#include <omp.h>
#include <vector>

int main(){

    std::vector<double> sum(10000);
    int nthreads = 1;

    #ifndef _OPENMP
        printf("serial summation\n");
    #else

    #pragma omp parallel
    {
    #pragma omp master
        {
        nthreads = omp_get_num_threads();
        std::cout << "omp summation with " << nthreads << " threads" << std::endl;
        }
    }
    #endif

    #ifndef _OPENMP
        for(std::size_t i=0; i<sum.size(); ++i){
            for(std::size_t j=0; j<10; ++j)
                sum[i] += 1;
        }
    #else
    #pragma omp parallel for
        for(std::size_t i=0; i<sum.size(); ++i){
            for(std::size_t j=0; j<10; ++j)
                sum[i] += 1;
        }
    #endif

    // for(std::size_t i=0; i<sum.size(); ++i){
    //     std::cout << "sum " << i << ": " << sum[i] << std::endl;
    // }
}