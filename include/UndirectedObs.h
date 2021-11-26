#ifndef __UNDIRECTED_OBS__
#define __UNDIRECTED_OBS__

#include <iostream>

enum class Bird_in_scope{none, foe, brother, both};

class UndirectedObs{

    protected:
        Bird_in_scope _b;

    public:
        static std::size_t size;
        UndirectedObs();
        UndirectedObs(Bird_in_scope b);
        //UndirectedObs(const Bird_in_scope &b);
        //UndirectedObs& operator=(const UndirectedObs &u);

        bool operator==(const UndirectedObs &u);
        
        //Setters
        static void set_size(std::size_t s) {size = s;}

        const inline Bird_in_scope get_bird_in_scope() const {return _b;}
        std::size_t to_int();
};

#endif