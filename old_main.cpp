/*
    CODE STILL UNDER DEVELOPMENT

    This code trains a user-defined number of (over) idealized birds in a pursuit-evasion scenario.
    A bird (the pursuer) learns how to chase the rest of the pack (evaders) given a certain
    representation of the state. 

    The state is defined as the positions and orientation of all the birds. The velocity module is fixed
    for every bird at every time step (v_pursuer > v_evader).

    To learn optimal policies, we are employing an actor critic algorithm with Natural policy gradient. 
    We are training the birds in an "autocurricula" fashion: the pursuer learns first, 
    after some episodes (fixed to 5000 in the code) it's time for all the evaders to learn concurrently.
    The natural policy gradient update is performed within Boltzmann.h file, while the actor critic
    algorithm is defined in this main file.

    The environment class defines the one-step dynamics and the rewards given to the agents, according to
    the actions they take.

    In the DirectedAgent.h class you can find how the observations are defined for the current implementation.
    Support for UndirectedObservation (i.e. without distinguishing whether other birds are pointing
    towards or outwards from the agent) is also available but main needs to be slightly modified.




*/


#include "State.h"
#include "Observable.h"
#include "Environment.h"
#include "Reward.h"
#include "RandomWalk.h"
#include "Boltzmann.h"
#include "Bird.h"
#include "V.h"
#include "Signal.h"
#include "Timer.h"
#include "Chase.h"
#include "UndirectedAgent.h"
#include "DirectedAgent.h"
#include <tuple>
#include <utility>
#include <string>
#include <list>
#include <memory>
#include <algorithm>

//Modify these typedefs in case you want to try a different observation mechanism

using Obs = Observable<DirectedObs>; //Observable<UndirectedObs>
using BoltzmannAgt = DirectedAgent<Boltzmann>; //UndirectedAgent<Boltzmann>

//You can also define a fixed policy Chase/Run with
//using BoltzmannAgt = UndirectedAgent<Chase>;

// Main execution
int main(){

    //*****************************************************************************************//
    //*****************************************************************************************//
    //                                    MODIFIABLE PART                                      //
    //*****************************************************************************************//
    //*****************************************************************************************//

    //Environment params
    constexpr double steering_angle_pursuer = M_PI/12;
    constexpr double steering_angle_evader = M_PI/10; 
    
    constexpr double v0_pursuer = 0.5; //v0_pursuer > v0_evader otherwise evader learns linear escape and it's game over
    constexpr double v0_evader = 0.4;

    constexpr double vision_range_pursuer = 50;
    constexpr double vision_range_evader = 10;
    constexpr double vision_angle_pursuer = M_PI;
    constexpr double vision_angle_evader = 3./2*M_PI;
    constexpr double capture_range = 0.5;
    constexpr double gamma = 1;

    //This can be modified: change setting to another possible Obs_setting
    //(foe_only, overwrite, both, closer)
    //foe_only means that both pursuer and evader can only see opposing species
    //overwrite means that both pursuer and evader prioritize the opponent (foe)
    //closer means that each bird sees the species of the closest bird in each sector
    //species in a sector, but can also see an individual of the same species (brother)
    const Obs_setting setting = Obs_setting::overwrite;
    std::size_t states_per_sector = 2;
    //In a sector you can have either an evader or a pursuer with these observation settings
    if(setting == Obs_setting::overwrite || setting == Obs_setting::closer)  
        states_per_sector = 3;
    else if(setting == Obs_setting::both)
        states_per_sector = 4;

    //Sets the corrects number of states per sector
    //For the class UndirectedObs
    UndirectedObs::set_size(states_per_sector); 

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

    //Instantiate a learning signal to alternate between preys and predator learning
    std::vector<std::size_t> learning_agent;
    for (auto i=1; i < static_cast<int>(episodes_num/2000); ++i){
        learning_agent.push_back(5000*i);
    }


    //*****************************************************************************************//
    //*****************************************************************************************//
    //                                UNMODIFIABLE PART                                        //
    //*****************************************************************************************//
    //*****************************************************************************************//

    if(is_directed)
        states_per_sector = states_per_sector*2; 
    
    const std::size_t state_space_dim = pow(states_per_sector, sectors_num);
    
    //Print the environment and training information into a log file
    std::ofstream log_file;
    log_file.open("data/env_info.csv");
    log_file << "episodes_num,episodes_length,num_of_birds,state_space_dim,episode_write_step,sectors,states_per_sector,is_directed" << std::endl;
    log_file << episodes_num << "," << episode_length << "," << num_of_birds << "," << 
             state_space_dim << "," << 1000 << "," << sectors_num << "," <<
             static_cast<int>(states_per_sector) << "," << (is_directed ? 1 : 0) << std::endl;


    Signal pred_training(learning_agent);

    //---------------------------------------------------------------------------------

    //State initialization
    Environment env(num_of_birds, v0_pursuer, v0_evader, capture_range, steering_angle_pursuer, steering_angle_evader);

    //Agents initialization
    std::vector<BoltzmannAgt> agents;
    for(std::size_t i=0; i < num_of_birds; ++i){
        agents.push_back(BoltzmannAgt(sectors_num, states_per_sector, vision_range_pursuer, vision_angle_pursuer));
    }
    
    //These features are shared by all the birds
    for(std::size_t i = 0; i < agents.size(); ++i ){
        agents[i].set_id(i);
        agents[i].set_policy(Boltzmann(state_space_dim, 3));
        agents[i].set_vision_angle(vision_angle_pursuer);
        agents[i].set_vision_sectors();
    }
    
    //These characterize only preys
    for(std::size_t j=1; j < agents.size(); ++j){
        agents[j].set_vision_range(vision_range_evader);
        agents[j].set_vision_angle(vision_angle_evader);
        agents[j].set_vision_sectors();
    }

    //Bunch of pointers to keep track of values during the run and avoiding hard copying
    std::shared_ptr<State> prev_state;
    std::shared_ptr<State> next_state;
    std::vector<Action> a(num_of_birds);
    std::vector<std::shared_ptr<Obs>> prev_obs(num_of_birds);
    std::vector<std::shared_ptr<Obs>> next_obs(num_of_birds);
    std::vector<V> v(num_of_birds, V(state_space_dim));
    std::vector<double> delta(num_of_birds);
    std::size_t ag_l; //The agent currently learning
    std::pair<Reward,bool> r; //Reward 

    //Storing data to open one single buffer at the end. Write to ram >> write to disk.
    std::list<std::pair<std::size_t,State>> traj;
    std::list<std::tuple<std::size_t,std::size_t, Obs>> record_obs;
    std::list<std::vector<std::size_t>> t_ep;
    std::vector<std::vector<double>> value_policy(4*num_of_birds, std::vector<double>(static_cast<unsigned int>(episodes_num*state_space_dim/1000)));
    
    //Output files of the simulation
    std::ofstream traj_file;
    std::ofstream obs_file;
    std::ofstream episode_file;
    std::vector<std::ofstream> value_policy_files(num_of_birds);

    //File initialization and header construction

    //---------------------------------------------------------------------------------
    traj_file.open("data/pursuer_trajectory.csv");

    //Header construction
    traj_file << "Episode" << ",";
    for(std::size_t i=0; i<num_of_birds; ++i){
        traj_file << "x" << i << ",y" << i << ",alpha" << i;
        if(i<num_of_birds-1) 
            traj_file << ",";
    }
    traj_file << "\n";


    obs_file.open("data/observations.csv");
    obs_file << "episode" << "," << "bird" << ",";

    for(std::size_t i=0; i<sectors_num-1; ++i){
        obs_file << "sec"+std::to_string(i)+",";
    }
    obs_file << "sec"+std::to_string(sectors_num-1)+"\n";
    

    episode_file.open("data/episode.csv");
    episode_file << "Episode,EndTime,PredatorTraining" << std::endl;

    for(std::size_t i=0; i<num_of_birds; ++i)
    {
     value_policy_files[i].open("data/value_policy"+std::to_string(i)+".csv");
     value_policy_files[i] << "value,";
     value_policy_files[i] << "left,";
     value_policy_files[i] << "straight,";
     value_policy_files[i] << "right";
     value_policy_files[i] << std::endl;
    }

    //---------------------------------------------------------------------------------

    //Time of episode
    std::size_t t = 0;
    
    //Here starts the actor-critic algorithm
    for(std::size_t ep=0; ep<episodes_num; ep++){

        //State initialization
        env.reset();
        prev_state = std::make_shared<State>(env.get_state());

        for(std::size_t i=0; i<agents.size(); ++i){
            prev_obs[i] = std::make_shared<Obs>(agents[i].obs(*prev_state, setting));
        }
        
        //This determines whose turn is it to learn (is 0, pursuer, else, evaders)
        ag_l = pred_training.step(ep);

        while(t < episode_length){

            //Data logging every 1000th episode
            if(ep%1000 == 0){
                traj.push_back(std::make_pair(ep,*prev_state));
                for(std::size_t i = 0; i<num_of_birds; ++i)
                    record_obs.push_back(std::make_tuple(ep,i,*prev_obs[i]));
            }
                
            //All agents get an observation based on the current state and return an action
            for(std::size_t i = 0; i < agents.size(); ++i){
                a[i] = (agents[i].act(*prev_state, *prev_obs[i]));
            }
            next_state = std::make_shared<State>(env.dynamics(a, *prev_state));
            
            for(std::size_t i=0; i<agents.size(); ++i)
                next_obs[i] = std::make_shared<Obs>(agents[i].obs(*next_state, setting));
  
            //r contains the reward for all the agents on its first components 
            //and a boolean variable stating if the episode is finished or not in its second component
            r = env.reward(*prev_state, static_cast<double>(episode_length), num_of_birds-1);

            //If episode is over:
            if(std::get<1>(r) == 1){
                for(std::size_t i=0; i<num_of_birds; ++i){
                    delta[i] = std::get<0>(r)[i] - v[i][*prev_obs[i]];
                    v[i][*prev_obs[i]] += alpha_w*delta[i]; //V values update
                }   
                //Theta values update
                if(ag_l != 0){ //Evaders are learning
                    for(std::size_t j=1; j<num_of_birds; ++j)
                        agents[j].update_policy(alpha_t*delta[j], *prev_obs[j], a[j]); 
                } else{ //Pursuer is learning
                    agents[0].update_policy(alpha_t*delta[0], *prev_obs[0], a[0]); 
                }                  
                break;
            }
            
            for(std::size_t i=0; i<num_of_birds; ++i){
                delta[i] = std::get<0>(r)[i] + gamma*v[i][*next_obs[i]] - v[i][*prev_obs[i]];
                v[i][*prev_obs[i]] += alpha_w*delta[i]; //V values update
            }     
         
            //Theta values updates
            if(ag_l == 0){
                agents[ag_l].update_policy(alpha_t*delta[ag_l], *prev_obs[ag_l], a[ag_l]);
            } else{
                for(std::size_t j=1; j<num_of_birds; ++j)
                    agents[j].update_policy(alpha_t*delta[j], *prev_obs[j], a[j]);
            }

            //State update
            prev_state = next_state;

            //Observation update
            for(std::size_t i=0; i<num_of_birds; ++i)
                prev_obs[i] = next_obs[i];

            //Update time step
            t++;
        } //End of a time step of algorithm

        //Logging value and policy data
        if(ep%1000 == 0){
            for(std::size_t i=0; i < num_of_birds; ++i){
                for(std::size_t k=0; k<state_space_dim; ++k){
                    value_policy[i*4][static_cast<int>(ep/1000)*state_space_dim+k] = v[i][k];
                    value_policy[i*4+1][static_cast<int>(ep/1000)*state_space_dim+k] = agents[i].get_policy()->get(k,0);
                    value_policy[i*4+2][static_cast<int>(ep/1000)*state_space_dim+k] = agents[i].get_policy()->get(k,1);
                    value_policy[i*4+3][static_cast<int>(ep/1000)*state_space_dim+k] = agents[i].get_policy()->get(k,2);
                }
            }
        } 

        t_ep.push_back(std::vector<std::size_t>{ep,t,ag_l});
        t = 0;
        
    } //End of an episode

    //Transferring data to file at the end of everything
    for(auto it1=traj.begin(); it1!=traj.end(); ++it1){
        traj_file << std::get<0>(*it1) << "," << std::get<1>(*it1); //<< "\n";
    }

    for(auto it1=record_obs.begin(); it1!=record_obs.end(); ++it1){
        obs_file << std::get<0>(*it1) << "," << std::get<1>(*it1) << ","
                 << std::get<2>(*it1) << std::endl;
    }
        

    for(auto it=t_ep.begin(); it!=t_ep.end(); ++it)
        episode_file << (*it)[0] << "," << (*it)[1] << "," << (*it)[2] << "\n";
  
    for(std::size_t i=0; i<num_of_birds; ++i){
        for(std::size_t k=0; k<value_policy[0].size(); ++k){
            value_policy_files[i] << value_policy[4*i][k] << ",";
            value_policy_files[i] << value_policy[4*i+1][k] << ",";
            value_policy_files[i] << value_policy[4*i+2][k] << ",";
            value_policy_files[i] << value_policy[4*i+3][k] << "\n";
        }
    }
    
    //Closing files
    traj_file.close();
    episode_file.close();
    for(std::size_t i=0; i<num_of_birds; ++i)
        value_policy_files[i].close();
    obs_file.close();

    return 0;
}

