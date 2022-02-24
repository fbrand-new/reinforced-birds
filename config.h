#include <math.h>

using Obs = Observable<bool>;
//using Observer = TilingObserver;
using Observer = ClosestObsDirected;
//using Observer = ClosestObsUndirected;
using Agt = Agent<Observer,Boltzmann,bool>;

//constexpr std::size_t neighbours = 3;
constexpr std::size_t pursuer_neighbours = 1;
constexpr std::size_t evader_neighbours = 1;

//Environment params
constexpr double steering_angle_pursuer = M_PI/12;
//constexpr double steering_angle_evader = M_PI/12; 
constexpr double steering_angle_evader = M_PI/14; 
//constexpr double steering_angle_evader = M_PI/16; 

// constexpr double v0_pursuer = 0.15; //v0_pursuer > v0_evader otherwise evader learns linear escape and it's game over
// // //constexpr double v0_evader = 0.10;
// constexpr double v0_evader = 0.12;

constexpr double v0_pursuer = 0.15;
//constexpr double v0_pursuer = 0.12;
constexpr double v0_evader = 0.12;

constexpr double pbc = 100;
constexpr double capture_range = 0.5;
constexpr double friends_range = 0.5;

//The first one is the pursuer, the other are agents
std::size_t num_of_birds = 11;

//Learning rates
double alpha_w = 0.01;
double alpha_t = 0.001;

//Decide the episode length
std::size_t episodes_num = 50000;
std::size_t episode_length = 500;

//Set the vision_sector for preys
//const std::vector<Angle> evader_meridians{-3.*M_PI/4, -2*M_PI/5, -M_PI/6, M_PI/6, 2*M_PI/5, 3.*M_PI/4};
//high angle
const std::vector<Angle> evader_meridians{-7.*M_PI/8, -3.*M_PI/4, -2*M_PI/5, -M_PI/6, M_PI/6, 2*M_PI/5, 3.*M_PI/4, 7.*M_PI/8};
//const std::vector<Angle> evader_meridians{-M_PI/2, -M_PI/3, -M_PI/6, M_PI/6, M_PI/3, M_PI/2};
//const std::vector<double> evader_parallels{6}; //This is really just the vision range
//const std::vector<double> evader_parallels{4.5}; //This is really just the vision range
//const double evader_vis_range{9};
const std::vector<double> evader_parallels{6}; //This is really just the vision range


const std::vector<Angle> pursuer_meridians{-M_PI/2, -M_PI/3, -M_PI/6, M_PI/6, M_PI/3, M_PI/2};
//const std::vector<double> pursuer_parallels{4,8}; //This is really just the vision range
const std::vector<double> pursuer_parallels{12}; //This is really just the vision range
//const double pursuer_vis_range{9};
//const std::vector<double> pursuer_parallels{6}; //This is really just the vision range

//This can be modified: change setting to another possible Obs_setting
//(foe_only, overwrite, both, closer)
//foe_only means that both pursuer and evader can only see opposing species
//overwrite means that both pursuer and evader prioritize the opponent (foe)
//closer means that each bird sees the species of the closest bird in each sector

// const Obs_setting setting = Obs_setting::overwrite;
// std::vector<std::size_t> states_per_sector = {2,3,3,3,2};
constexpr int step_write = 2000;

constexpr double av_dist = 7;
constexpr double prey_repulsion = 0;