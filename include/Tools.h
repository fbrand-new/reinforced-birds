#ifndef __TOOLS_H_
#define __TOOLS_H_

#include <fstream>

class Tools{

    public:
        template<class ... Types>
        static std::ofstream& build_header(std::ofstream &s, std::string k, Types ... args);
};

#endif