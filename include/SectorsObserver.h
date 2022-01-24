#ifndef __SECTORS_OBSERVER_H__
#define __SECTORS_OBSERVER_H__

#include "Observable.h"
#include "State.h"

template <class T>
class SectorObserver{

    protected:
        std::vector<Angle> _meridians;
        std::vector<double> _parallels;
        double _vision_range;
        std::size_t _tiles_num; //Number of sectors
        std::size_t _me_id;
        double _pbc;

    public:
        SectorObserver(std::vector<Angle> meridians, std::vector<double> parallels, std::size_t me_id, double pbc):
        _meridians{meridians},
        _parallels{parallels},
        _vision_range{_parallels[_parallels.size()-1]},
        _tiles_num{(_meridians.size()-1)*_parallels.size()},
        _me_id{me_id},
        _pbc{pbc}
        {}

        Observable<bool> obs(const State &s) {return static_cast<T*>(this)->obs(s);}

};

#endif