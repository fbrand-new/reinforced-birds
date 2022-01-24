#ifndef __CLOSEST_OBS_DIRECTED__H__
#define __CLOSEST_OBS_DIRECTED_H__

#include "Observable.h"
#include "State.h"
#include "SectorsObserver.h"

class ClosestObsDirected : SectorObserver<ClosestObsDirected>{

    public:
        ClosestObsDirected(std::vector<Angle> meridians, std::vector<double> parallels, std::size_t me_id, double pbc):
            SectorObserver<ClosestObsDirected>(meridians,parallels,me_id,pbc)
            {}

        Observable<bool> obs(const State &s);

};

#endif