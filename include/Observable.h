#ifndef __OBSERVABLE__
#define __OBSERVABLE__

#include <memory>
#include <fstream>

class Observable{

    private:
        std::shared_ptr<unsigned int []> _o;
        std::size_t _sectors;

    public:
        Observable();
        Observable(unsigned int size);

        std::shared_ptr<unsigned int []> get_sectors();
        std::size_t get_sectors_num();

        void empty_sector(unsigned int i);
        void non_empty_sector(unsigned int i);
};

std::ostream& operator <<(std::ostream & os, Observable &o);

#endif