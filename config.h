#include <math.h>
// #include "Agent.h"

constexpr auto states_per_sector = 2;

//Environment params
constexpr double steering_angle_pursuer = M_PI/12;
constexpr double steering_angle_evader = M_PI/12; 
//constexpr double steering_angle_evader = M_PI/14; 
constexpr double v0_pursuer = 0.15; //v0_pursuer > v0_evader otherwise evader learns linear escape and it's game over
// constexpr double v0_evader = 0.12;
constexpr double v0_evader = 0.15;

constexpr double pbc = 10;
// constexpr double vision_range_pursuer = 50;
// constexpr double vision_range_evader = 10;
// constexpr double vision_angle_pursuer = M_PI;
// constexpr double vision_angle_evader = 3./2*M_PI;
constexpr double capture_range = 0.5;
//constexpr double gamma = 1; we are using an undiscounted version of the RL problem

//Set this to 0 if we have undirected observations
//bool is_directed = 1;

//Decide the number of birds. 
//The first one is the pursuer, the other are agents
std::size_t num_of_birds = 11;

//Learning rates
double alpha_w = 0.01;
double alpha_t = 0.001;

//Decide the episode length
std::size_t episodes_num = 40000;
std::size_t episode_length = 500;

//Set the vision_sector for preys
const std::vector<Angle> evader_meridians{-3.*M_PI/4, -M_PI/3, -M_PI/6, M_PI/6, M_PI/3, 3.*M_PI/4};
const std::vector<double> evader_parallels{1.5,4}; //This is really just the vision range
//const double evader_vis_range{9};
//const std::vector<double> evader_parallels{3}; //This is really just the vision range


const std::vector<Angle> pursuer_meridians{-M_PI/2, -M_PI/3, -M_PI/6, M_PI/6, M_PI/3, M_PI/2};
//const std::vector<double> pursuer_parallels{4,8}; //This is really just the vision range
const std::vector<double> pursuer_parallels{5,9}; //This is really just the vision range
//const double pursuer_vis_range{9};
//const std::vector<double> pursuer_parallels{6}; //This is really just the vision range

//This can be modified: change setting to another possible Obs_setting
//(foe_only, overwrite, both, closer)
//foe_only means that both pursuer and evader can only see opposing species
//overwrite means that both pursuer and evader prioritize the opponent (foe)
//closer means that each bird sees the species of the closest bird in each sector

// const Obs_setting setting = Obs_setting::overwrite;
// std::vector<std::size_t> states_per_sector = {2,3,3,3,2};
