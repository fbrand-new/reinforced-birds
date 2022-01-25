#ifndef __CLOSEST_OBS_DIRECTED__H__
#define __CLOSEST_OBS_DIRECTED_H__

#include "Observable.h"
#include "State.h"
#include "SectorsObserver.h"
#include <map> 
#include <algorithm>

class ClosestObsDirected : SectorObserver<ClosestObsDirected>{

    private:
        std::size_t _neighbours;

    public:
        ClosestObsDirected(std::vector<Angle> meridians, std::vector<double> parallels, std::size_t me_id, std::size_t neighbours, double pbc):
            SectorObserver<ClosestObsDirected>(meridians,parallels,me_id,pbc),
            _neighbours{neighbours}
            {}

        Observable<bool> obs(const State &s);

};

#endif