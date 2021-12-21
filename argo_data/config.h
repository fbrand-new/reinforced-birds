#include <math.h>
#include "BaseAgent.h"

//Environment params
constexpr double steering_angle_pursuer = M_PI/12;
constexpr double steering_angle_evader = M_PI/10; 
constexpr double v0_pursuer = 0.5; //v0_pursuer > v0_evader otherwise evader learns linear escape and it's game over
constexpr double v0_evader = 0.4;
constexpr double vision_range_pursuer = 50;
constexpr double vision_range_evader = 5;
constexpr double vision_angle_pursuer = M_PI;
constexpr double vision_angle_evader = 2*M_PI;
constexpr double capture_range = 0.5;
//constexpr double gamma = 1; we are using an undiscounted version of the RL problem

//This can be modified: change setting to another possible Obs_setting
//(foe_only, overwrite, both, closer)
//foe_only means that both pursuer and evader can only see opposing species
//overwrite means that both pursuer and evader prioritize the opponent (foe)
//closer means that each bird sees the species of the closest bird in each sector
//species in a sector, but can also see an individual of the same species (brother)
const Obs_setting setting = Obs_setting::overwrite;

//Set this to 0 if we have undirected observations
bool is_directed = 1;

//Decide the number of birds. 
//The first one is the pursuer, the other are agents
std::size_t num_of_birds = 10;

//Decide the number of vision sectors each agent observes
std::size_t sectors_num = 5;

//Learning rates
double alpha_w = 0.01;
double alpha_t = 0.001;

//Decide the episode length
std::size_t episodes_num = 200000;
std::size_t episode_length = 500;

//Decide prey vision sectors
std::vector<double> prey_sectors{-M_PI, -3.*M_PI/4, -M_PI/6, M_PI/6, 3.*M_PI/4, M_PI};
