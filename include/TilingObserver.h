#ifndef __TILING_H__
#define __TILING_H__

#include "Observable.h"
#include "Angle.h"
#include "State.h"
#include <tuple>

//TODO: we actually need to have 3 grids "overlapping" each other.
//In the first one we check whether we have or not an evader alone
//In the second one we check whether we have or not a pursuer alone
//In the third we check if they are both present or not.
//enum class Tile{none, evader, pursuer, both};

class TilingObserver{

    private:
        std::vector<Angle> _meridians;
        std::vector<double> _parallels;
        double _vision_range;
        std::size_t _tiles_num;
        std::size_t _me_id;
        double _pbc; //Has to be the same as in environment
        std::tuple<bool,double,Angle> single_obs(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha);

    public:
        TilingObserver();
        TilingObserver(std::vector<Angle> meridians, std::vector<double> parallels, std::size_t me_id, double pbc);
        Observable<bool> obs(State &s);

};

#endif
