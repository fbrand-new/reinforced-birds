#ifndef __DIRECTED_AGENT_H__
#define __DIRECTED_AGENT_H__

#include "BaseAgent.h"
#include "Boltzmann.h"

template <typename Policy>
class DirectedAgent : public BaseAgent<DirectedSector, Policy>{

    private:

    public:

        //Constructors
        DirectedAgent();
        DirectedAgent(Policy &p);
        DirectedAgent(double vision_range, double vision_angle);
        DirectedAgent(std::size_t sector_num, std::vector<std::size_t> state_per_sector, double vision_range, double vision_angle);
        DirectedAgent(Policy &p, std::size_t sector_num, std::vector<std::size_t> state_per_sector, double vision_range, double vision_angle);
        DirectedAgent(DirectedAgent &&a) = default;

        // Update policy given an observable and action, we use this only with softmax policy
        void update_policy(double coeff, Observable<DirectedSector> &o, Action &a);

        //method to be called in the main
        Observable<DirectedSector> obs(State &s, Obs_setting setting);

        void obs_closer(const Bird &me, const std::vector<Bird> &birds, double sin_alpha, double cos_alpha, std::size_t pursuers_num, std::size_t me_id);
        void obs_opponent(std::size_t begin, std::size_t end, const Bird &me, const std::vector<Bird> &birds, double sin_alpha, double cos_alpha);
        void obs_brother(std::size_t begin, std::size_t end, const Bird &me, const std::vector<Bird> &birds, double sin_alpha, double cos_alpha, Obs_setting setting, const std::size_t me_id);
        std::tuple<bool,Angle,Angle> out_of_scope(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha);

        //Utilities functions
        static std::pair<Angle, Angle> relative_angle_and_orientation(const Bird &a, const Bird &b, const double sin_alpha, const double cos_alpha);

};

template <typename Policy>
DirectedAgent<Policy>::DirectedAgent(): 
    BaseAgent<DirectedSector, Policy>()
    {}

template <typename Policy>
DirectedAgent<Policy>::DirectedAgent(Policy &p): 
    BaseAgent<DirectedSector,Policy>(p)
    {}

template <typename Policy>
DirectedAgent<Policy>::DirectedAgent(double vision_range, double vision_angle):
    BaseAgent<DirectedSector, Policy>(vision_range, vision_angle)
    {}

template <typename Policy>
DirectedAgent<Policy>::DirectedAgent(std::size_t sector_num, std::vector<std::size_t> state_per_sector, double vision_range, double vision_angle):
    BaseAgent<DirectedSector, Policy>(sector_num, state_per_sector,vision_range,vision_angle)
    {}

template <typename Policy>
std::pair<Angle,Angle> DirectedAgent<Policy>::relative_angle_and_orientation(const Bird &a, const Bird &b, const double sin_alpha, const double cos_alpha){
  
    auto rel_angle = Angle(relative_angle(a,b,sin_alpha,cos_alpha));
    auto rel_alpha =  Angle(b.get_alpha() - a.get_alpha() - rel_angle); //b is rotated to the relative x axis (of a)

    return std::make_pair(rel_angle, rel_alpha);
}

template <typename Policy>
std::tuple<bool, Angle, Angle> DirectedAgent<Policy>::out_of_scope(const Bird &me, const Bird &b, const double sin_alpha, const double cos_alpha){

    if (relative_distance_squared(me, b) > this->_vision_range*this->_vision_range){
        return std::make_tuple(1, Angle(), Angle()); //Since we are out of scope, Angles are just placeholders
    }
        
    //TODO: we can marginally do a better job if we compute the angle first, check whether 
    //the other bird is in scope and then compute the relative orientation
    auto angle_and_orientation = relative_angle_and_orientation(me, b,sin_alpha, cos_alpha);
  
    if(angle_and_orientation.first.get() < this->_vision_sectors[0]){
        return std::make_tuple(1, Angle(), Angle()); //Since we are out of scope, Angles are just placeholders
    }
        
    //TODO: We may add a check on the other hand of the scope also, i.e. angle > vision_sector[-1]

    return std::make_tuple(0, angle_and_orientation.first, angle_and_orientation.second);
}

template<typename Policy>
void DirectedAgent<Policy>::obs_closer(const Bird &me, const std::vector<Bird> &birds, double sin_alpha, double cos_alpha, std::size_t pursuers_num, std::size_t me_id){
    
    std::vector<std::pair<std::size_t, double>> sorted_birds;
    double dist;

    //We insert in the array only birds that are in vision, no need to sort the others
    //Two for loops are faster than having an if condition to check in a loop every time
    for(std::size_t i=0; i<me_id; ++i){
        dist = relative_distance_squared(me, birds[i]);
        if(dist < this->_vision_range*this->_vision_range){
            sorted_birds.push_back(std::make_pair(i,dist));
        }
    }
    for(std::size_t i=me_id+1; i<birds.size(); ++i){
       dist = relative_distance_squared(me, birds[i]);
        if(dist < this->_vision_range*this->_vision_range){
            sorted_birds.push_back(std::make_pair(i,dist));
        }
    }

    //Sort the array of birds who made it into our vision
    std::sort(sorted_birds.begin(), sorted_birds.end(), [](auto &left, auto &right) {
        return left.second < right.second;
    });

    for(auto &b:sorted_birds){
        auto angle_and_orientation = relative_angle_and_orientation(me,birds[b.first],sin_alpha, cos_alpha);
        
        if(angle_and_orientation.first.get() < this->_vision_sectors[0]) //If he is behind our vision
            continue; //Go to the next bird
        
        for(std::size_t i=0; i<this->_o.get_sectors_num(); ++i){
            if(this->_o.is_sector_empty(i)){
                if(angle_and_orientation.first.get() < this->_vision_sectors[i+1]){
                    //If he is in the sector we identify the species and its orientation
                    auto orientation = angle_and_orientation.second.get();
                    bool dir_out; 
                    if(orientation < -M_PI_2 || orientation > M_PI_2){
                        dir_out = 0;
                    } else {
                        dir_out = 1;
                    }

                    if(b.first < pursuers_num){
                        if(me.get_species()==Species::evader)
                            // this->_o.set_sector(i, DirectedObs(Bird_in_scope::foe, dir));
                            this->_o.set_sector(i, dir_out ? DirectedSector::foe_out : DirectedSector::foe_in);
                        else
                            // this->_o.set_sector(i, DirectedObs(Bird_in_scope::brother, dir));
                            this->_o.set_sector(i, dir_out ? DirectedSector::brother_out : DirectedSector::brother_in);
                    } else {
                        if(me.get_species()==Species::evader)
                            this->_o.set_sector(i, dir_out ? DirectedSector::brother_out : DirectedSector::brother_in);
                        else
                            this->_o.set_sector(i, dir_out ? DirectedSector::foe_out : DirectedSector::foe_in);
                    }      
                    break;  
                } //Close sector if
            } //Close empty sector if
        } //Close for
    } //Close sorted birds for 

}

template<typename Policy>
void DirectedAgent<Policy>::obs_opponent(std::size_t begin, std::size_t end, const Bird &me, const std::vector<Bird> &birds, double sin_alpha, double cos_alpha){
    
    //Set of birds that are yet to be observed by this agent
    std::map<std::size_t, std::tuple<bool,Angle,Angle>> birds_to_locate; 
    //Here we full the map above with all the birds and then we remove them as soon as they are observed
    for(std::size_t i=begin; i<end; ++i){ 
        auto oos = out_of_scope(me, birds[i], sin_alpha, cos_alpha);
        if(!std::get<0>(oos))
            birds_to_locate.emplace(i,oos);
    }

    //We decide on average whether more foes are pointing out or in with respect to us
    int in = 0;
    int out = 0;

    for(std::size_t i = 0; i < this->_o.get_sectors_num(); ++i){
        for(auto it = birds_to_locate.cbegin(); it != birds_to_locate.cend();){    //Cycle through unobserved birds           
            if(std::get<1>((*it).second).get() < this->_vision_sectors[i+1]){ //If the angle of bird b falls into sector i
                auto orientation = std::get<2>((*it).second).get();
                if( orientation < -M_PI_2 || orientation > M_PI_2){ 
                    in++; //Pointing towards a
                } else {
                    out++; //Pointing outwards from a
                }
                it = birds_to_locate.erase(it); //Birds has been seen, so it is removed from the map
            } else {
                ++it;
            }
        }

        if(in>out || (in == out && in>0)) //If we have strictly more birds looking inwards rather than outwards
            // this->_o.set_sector(i,DirectedObs(Bird_in_scope::foe,Direction::in));
            this->_o.set_sector(i,DirectedSector::foe_in);
        else if (in < out)
            // this->_o.set_sector(i,DirectedObs(Bird_in_scope::foe,Direction::out));
            this->_o.set_sector(i,DirectedSector::foe_out);

        in = 0;
        out = 0;
    }
    
}

template<typename Policy>
void DirectedAgent<Policy>::obs_brother(std::size_t begin, std::size_t end, const Bird &me, const std::vector<Bird> &birds, double sin_alpha, double cos_alpha, Obs_setting setting, const std::size_t me_id){
    
    //Set of birds that are yet to be observed by this agent
    std::map<std::size_t, std::tuple<bool,Angle,Angle>> birds_to_locate;
    //Here we full the map above with all the birds and then we remove them as soon as they are observed
    for(std::size_t i=begin; i<end; ++i){
        if(i != me_id){ //TODO: if we don't put extra checks there will be some overhead for pursuer(s)
            auto oos = out_of_scope(me, birds[i], sin_alpha, cos_alpha);
            if(!std::get<0>(oos))
                birds_to_locate.emplace(i,oos);
        }
    }

    //We decide on average whether more foes are pointing out or in with respect to us
    int in = 0;
    int out = 0;

    for(std::size_t i = 0; i < this->_o.get_sectors_num(); ++i){
        if(setting == Obs_setting::overwrite){ //If we prioritize opponent first and if the sector is already occupied we leave it as is
            // if(this->_o.get_sector(i).get_bird_in_scope() == Bird_in_scope::foe)
            //     continue;
            if(this->_o.get_sector(i) == DirectedSector::foe_in || this->_o.get_sector(i) == DirectedSector::foe_out)
                continue;
        }
        for(auto it = birds_to_locate.cbegin(); it != birds_to_locate.cend();){              
            if(std::get<1>((*it).second).get() < this->_vision_sectors[i+1]){ //If the angle of bird b falls into sector i
                auto orientation = std::get<2>((*it).second).get();
                if( orientation < -M_PI_2 || orientation > M_PI_2){ //Pointing towards a
                    in++;
                } else {
                    out++;
                }
                it = birds_to_locate.erase(it);
            } else {
                ++it;
            }
        }

        if(in>out || (in == out && in>0)){ //If we have strictly more birds looking inwards rather than outwards
            if(setting == Obs_setting::both)
                // this->_o.set_sector(i,DirectedObs(Bird_in_scope::both,Direction::in));
                this->_o.set_sector(i,DirectedSector::both);
            else
                // this->_o.set_sector(i,DirectedObs(Bird_in_scope::brother,Direction::in));
                this->_o.set_sector(i,DirectedSector::brother_in);
        }
        else if (in < out){
            if(setting == Obs_setting::both)
                // this->_o.set_sector(i,DirectedObs(Bird_in_scope::both,Direction::out));   
                this->_o.set_sector(i,DirectedSector::both);  
            else
                // this->_o.set_sector(i,DirectedObs(Bird_in_scope::brother,Direction::in));
                this->_o.set_sector(i,DirectedSector::brother_out);
        }

        in = 0;
        out = 0;
    }
    
}


//If we want an average direction we need to cycle through sectors first and then through birds.
template <typename Policy>
Observable<DirectedSector> DirectedAgent<Policy>::obs(State &s, Obs_setting setting){

    std::size_t pursuers_num = s.get_pursuer_num();
    std::size_t me_id = this->get_id();

    auto birds = s.get_birds();
    auto me = birds[me_id];

    //Sin and cos of this agent orientation, in order to rotate the system towards him
    double sin_alpha = -Angle::sin(me.get_alpha());
    double cos_alpha = Angle::cos(me.get_alpha());

    //Reset observation
    for(std::size_t i=0; i<this->_o.get_sectors_num(); ++i)
        this->_o.set_sector(i,DirectedSector::none);
 
    if(setting == Obs_setting::closer){
        obs_closer(me, birds, sin_alpha, cos_alpha, pursuers_num, me_id);
        return this->_o;
    }

    //Sees opponents and sets sectors
    if(me.get_species() == Species::pursuer)
        obs_opponent(pursuers_num, birds.size(), me, birds, sin_alpha, cos_alpha);
    else
        obs_opponent(0,pursuers_num, me, birds, sin_alpha, cos_alpha);

    if(setting == Obs_setting::foe_only)
        return this->_o;

    if(me.get_species() == Species::pursuer){
        obs_brother(0,pursuers_num, me, birds, sin_alpha, cos_alpha, setting, me_id);
    } else {
        obs_brother(pursuers_num, birds.size(), me, birds, sin_alpha, cos_alpha, setting, me_id);
    }

    return this->_o;
}



#endif