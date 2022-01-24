#ifndef __CLOSEST_OBSERVER_H__
#define __CLOSEST_OBSERVER_H__

#include "Observable.h"
#include "State.h"
#include "SectorsObserver.h"

class ClosestObserver : SectorObserver<ClosestObserver>{

    // private:
    //     std::vector<Angle> _meridians;
    //     double _vision_range;
    //     std::size_t _d; //Number of sectors
    //     std::size_t _me_id;
    //     double _pbc;

    public:
        ClosestObserver(std::vector<Angle> meridians, std::vector<double> parallels, std::size_t me_id, double pbc):
            SectorObserver<ClosestObserver>(meridians,parallels,me_id,pbc)
            {}
        Observable<bool> obs(const State &s);
    //     ClosestObserver(std::vector<Angle> meridians, double vis_range, std::size_t size, std::size_t me_id, double pbc);
    //     Observable<bool> obs(const State &s);

};

#endif