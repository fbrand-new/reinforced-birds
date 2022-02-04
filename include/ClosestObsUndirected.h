#ifndef __CLOSEST_OBS_UNDIRECTED_H_
#define __CLOSEST_OBS_UNDIRECTED_H__

#include "Observable.h"
#include "State.h"
#include "SectorsObserver.h"
#include <map> 
#include <algorithm>

class ClosestObsUndirected : SectorObserver<ClosestObsUndirected>{

    private:
        std::size_t _neighbours;

    public:
        ClosestObsUndirected(std::vector<Angle> meridians, std::vector<double> parallels, std::size_t me_id, std::size_t neighbours, double pbc):
            SectorObserver<ClosestObsUndirected>(meridians,parallels,me_id,pbc),
            _neighbours{neighbours}
            {}

        Observable<bool> obs(const State &s);

};

#endif