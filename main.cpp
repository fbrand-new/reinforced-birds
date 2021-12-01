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
#include <Eigen/Dense>

/*
KISS
Define:
- A: Space of Actions
- O: Space of Observables. 
    For now it is going to be defined by k sectors, each one being either empty (0) or non-empty(1)
- p(s'|a,s): One step dynamics
- o(s): Observables
- R(s,a,s'): Rewards -> Here we just going to have R(s')
*/
// ----------------------------------- ---

//Modify these typedefs in case you want to try a different observation mechanism

using Obs = Observable<DirectedObs>;
using BoltzmannAgt = DirectedAgent<Boltzmann>;
//using ChaseAgt = UndirectedAgent<Chase>;

// Main execution
int main(){

    //Environment params
    double steering_angle = M_PI/6;
    double vision_range = 15;
    double vision_angle = M_PI;
    double capture_range = 0.5;
    constexpr double gamma = 1;

    const Obs_setting setting = Obs_setting::overwrite;
    std::size_t states_per_sector = 2;
    
    if(setting == Obs_setting::overwrite)
        states_per_sector = 3;
    else if(setting == Obs_setting::both)
        states_per_sector = 4;

    //Sets the corrects number of states per sector
    //For the class UndirectedObs
    UndirectedObs::set_size(states_per_sector); 

    //This is in case we have directed observations
    states_per_sector = states_per_sector*2; 

    //Decide the number of birds. 
    //Each of them is an agent, some of them will just use a fixed policy
    //The first one is the pursuer, the other are agents
    std::size_t num_of_birds = 2;
    std::size_t sectors_num = 5;
    
    const std::size_t state_space_dim = pow(states_per_sector, sectors_num);
    
    //Learning rates
    double alpha_w = 0.01;
    double alpha_t = 0.001;

    //Decide the episode length
    std::size_t episodes_num = 200000;
    std::size_t episode_length = 500;

    //Print the environment and training information into a log file
    std::ofstream log_file;
    log_file.open("data/env_info.csv");
    log_file << "episodes_num,episodes_length,num_of_birds,state_space_dim,episode_write_step,sectors,states_per_sector,is_directed" << std::endl;
    log_file << episodes_num << "," << episode_length << "," << num_of_birds << "," << 
             state_space_dim << "," << 1000 << "," << sectors_num << "," <<
             static_cast<int>(sqrt(states_per_sector)) << "," << 1 <<  std::endl;

    //Instantiate a learning signal to alternate between preys and predator learning
    std::vector<std::size_t> learning_agent;
    for (auto i=1; i < static_cast<int>(episodes_num/2000); ++i){
        learning_agent.push_back(10000*i);
    }
    Signal pred_training(learning_agent);

    //---------------------------------------------------------------------------------

    //State initialization
    Environment env(num_of_birds, 0.2, capture_range, steering_angle); //How many birds

    //Agents initialization
    //TODO: kind of confusing as of now
    std::vector<BoltzmannAgt> agents;
    for(std::size_t i=0; i < num_of_birds; ++i){
        agents.push_back(BoltzmannAgt(sectors_num, states_per_sector, vision_range, vision_angle));
    }
    
    for(std::size_t i = 0; i < agents.size(); ++i ){
        //agents[i].set_vision_range(15);
        agents[i].set_id(i);
        agents[i].set_policy(Boltzmann(state_space_dim, 3));
        agents[i].set_vision_angle(M_PI);
        agents[i].set_vision_sectors();
    }
    
    for(std::size_t j=1; j < agents.size(); ++j){
        agents[j].set_vision_angle(3./2*M_PI);
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
    //std::vector<std::vector<double>> value_policy(static_cast<unsigned int>(episodes_num*state_space_dim/1000), std::vector<double>(4*num_of_birds));
    std::vector<std::vector<double>> value_policy(4*num_of_birds, std::vector<double>(static_cast<unsigned int>(episodes_num*state_space_dim/1000)));
    

    //Output files of the simulation
    std::ofstream traj_file;
    std::ofstream obs_file;
    std::ofstream episode_file;
    //std::ofstream value_policy_file;
    std::vector<std::ofstream> value_policy_files(num_of_birds);

    //File initialization
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

    // value_policy_file.open("data/value_policy.csv");
    // for(std::size_t i=0; i<num_of_birds-1; i++){
    //     value_policy_file << "value_" + std::to_string(i) << ",";
    //     value_policy_file << "left_" + std::to_string(i) << ",";
    //     value_policy_file << "straight_" + std::to_string(i) << ",";
    //     value_policy_file << "right_" + std::to_string(i) << ",";
    // }

    // value_policy_file << "value_" + std::to_string(num_of_birds-1) << ",";
    // value_policy_file << "left_" + std::to_string(num_of_birds-1) << ",";
    // value_policy_file << "straight_" + std::to_string(num_of_birds-1) << ",";
    // value_policy_file << "right_" + std::to_string(num_of_birds-1) << std::endl;

    //Time of episode
    std::size_t t = 0;
    
    //We should also loop on multiple episodes
    for(std::size_t ep=0; ep<episodes_num; ep++){

        //State initialization
        env.reset();
        prev_state = std::make_shared<State>(env.get_state());

        for(std::size_t i=0; i<agents.size(); ++i){
            prev_obs[i] = std::make_shared<Obs>(agents[i].obs(*prev_state, setting));
        }
        
        //ag_l = pred_training.step(ep);
        // if(ep < 100000)
            ag_l = pred_training.step(ep);
        // else
        //     ag_l = 1;

        while(t < episode_length){

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
  
            //TODO: conceptually it does not make sense that reward needs a template
            r = env.reward(*prev_state, static_cast<double>(episode_length), num_of_birds-1);

            //Check if episode is over:
            if(std::get<1>(r) == 1){
                for(std::size_t i=0; i<num_of_birds; ++i){
                    delta[i] = std::get<0>(r)[i] - v[i][*prev_obs[i]];
                    v[i][*prev_obs[i]] += alpha_w*delta[i]; //V values update
                }   
                //Theta values update
                if(ag_l != 0){
                    for(std::size_t j=1; j<num_of_birds; ++j)
                        agents[j].update_policy(alpha_t*delta[j], *prev_obs[j], a[j]); 
                } else{
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
        }

        // if(ep%1000 == 0){
        //     for(std::size_t i=0; i < num_of_birds; ++i){
        //         for(std::size_t k=0; k<state_space_dim; ++k){
        //             value_policy[static_cast<int>(ep/1000)*state_space_dim+k][i*4] = v[i][k];
        //             value_policy[static_cast<int>(ep/1000)*state_space_dim+k][i*4+1] = agents[i].get_policy()->get(k,0);
        //             value_policy[static_cast<int>(ep/1000)*state_space_dim+k][i*4+2] = agents[i].get_policy()->get(k,1);
        //             value_policy[static_cast<int>(ep/1000)*state_space_dim+k][i*4+3] = agents[i].get_policy()->get(k,2);
        //         }
        //     }
        // }

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
        
    }

    for(auto it1=traj.begin(); it1!=traj.end(); ++it1){
        traj_file << std::get<0>(*it1) << "," << std::get<1>(*it1); //<< "\n";
    }

    for(auto it1=record_obs.begin(); it1!=record_obs.end(); ++it1){
        obs_file << std::get<0>(*it1) << "," << std::get<1>(*it1) << ","
                 << std::get<2>(*it1) << std::endl;
    }
        

    for(auto it=t_ep.begin(); it!=t_ep.end(); ++it)
        episode_file << (*it)[0] << "," << (*it)[1] << "," << (*it)[2] << "\n";

    // for(std::size_t i=0; i<value_policy.size(); ++i){
    //     for(std::size_t j=0; j<4*num_of_birds-1; ++j)
    //         value_policy_file << value_policy[i][j] << ",";
    //     value_policy_file << value_policy[i][4*num_of_birds-1] << "\n";
    // }

    
    for(std::size_t i=0; i<num_of_birds; ++i){
        for(std::size_t k=0; k<value_policy[0].size(); ++k){
            value_policy_files[i] << value_policy[4*i][k] << ",";
            value_policy_files[i] << value_policy[4*i+1][k] << ",";
            value_policy_files[i] << value_policy[4*i+2][k] << ",";
            value_policy_files[i] << value_policy[4*i+3][k] << "\n";
        }
    }
    


    traj_file.close();
    episode_file.close();

    for(std::size_t i=0; i<num_of_birds; ++i)
        value_policy_files[i].close();

    obs_file.close();
    return 0;
}

