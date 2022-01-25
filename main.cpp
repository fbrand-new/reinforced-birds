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
#include "Agent.h"
#include "TilingObserver.h"
#include "ClosestObserver.h"
#include "ClosestObsDirected.h"
#include <tuple>
#include <utility>
#include <string>
#include <list>
#include <memory>
#include <algorithm>
#include <random>
#include <omp.h>

#include "config.h"

using Obs = Observable<bool>;
//using Observer = TilingObserver;
using Observer = ClosestObsDirected;
using Agt = Agent<Observer,Boltzmann,bool>;


int main(){

    //Environment parameters and hyperparameters of the problem are defined in config.h
    //and included before the main function
    //Decide the number of vision sectors each agent observes
    const std::size_t sectors_num = (evader_meridians.size()-1)*evader_parallels.size();
    
    //Linear approx
    // const auto state_space_dim = sectors_num*states_per_sector+1;
    //Non linear approx
    //const std::size_t pursuer_state_space_dim = sectors_num+1;
    //const std::size_t state_space_dim = (2*sectors_num+1)*(2*sectors_num+1);

    //For each bird we consider, we have every possible sector 
    //twice (ingoing and outgoing) and the "null" sector
    const std::size_t state_space_dim = pow(2*sectors_num+1,1+neighbours);

    //Instantiate a learning signal to alternate between preys and predator learning
    
    // 10-1: I want to try to have purely concurrent learning
    // std::vector<std::size_t> learning_agent;
    // for (auto i=1; i < static_cast<int>(episodes_num/2000); ++i){
    //     learning_agent.push_back(5000*i);
    // }
    
    //Print the environment and training information into a log file
    std::ofstream log_file;
    log_file.open("data/env_info.csv");
    log_file << "episodes_num,episodes_length,num_of_birds,state_space_dim,episode_write_step,sectors" << std::endl;
    log_file << episodes_num << "," << episode_length << "," << num_of_birds << "," << 
             state_space_dim << "," << 1000 << "," << sectors_num  << std::endl;

    // std::ofstream sectors_file;
    // sectors_file.open("data/sectors_info.csv");
    // for(std::size_t i=0; i<sectors_num-1; ++i)
    //     sectors_file << "bot_angle_"+std::to_string(i)+","+"states_sector_"+std::to_string(i)+",";
    // sectors_file << "bot_angle_"+std::to_string(sectors_num-1)+","+"states_sector_"+std::to_string(sectors_num-1);


    // Signal pred_training(learning_agent);

    //Instantiation of the random number generator for the program
    std::random_device rd;
    std::mt19937 rng(rd());

    //---------------------------------------------------------------------------------

    //Initialization of environment and agents

    Environment env(num_of_birds, v0_pursuer, v0_evader, friends_range, capture_range, steering_angle_pursuer, steering_angle_evader,pbc);
    std::vector<Agt> agents;
    agents.push_back(Agt(Boltzmann(state_space_dim,3),Observer(pursuer_meridians,pursuer_parallels,0,neighbours,pbc)));
    //agents.push_back(Agt(Boltzmann(state_space_dim,3),Observer(pursuer_meridians,pursuer_vis_range,sectors_num,0,pbc)));

    for(std::size_t i=1; i < num_of_birds; ++i){
        agents.push_back(Agt(Boltzmann(state_space_dim,3),Observer(evader_meridians, evader_parallels,i,neighbours,pbc)));
    }

    //Bunch of pointers to keep track of values during the run and avoiding hard copying
    std::shared_ptr<State> prev_state;
    std::shared_ptr<State> next_state;
    std::vector<Action> a(num_of_birds);
    std::vector<std::shared_ptr<Obs>> prev_obs(num_of_birds);
    std::vector<std::shared_ptr<Obs>> next_obs(num_of_birds);
    std::vector<V> v(num_of_birds, V(state_space_dim));
    std::vector<double> delta(num_of_birds);
    // std::size_t ag_l; //The agent currently learning
    std::pair<Reward,bool> r; //Reward 

    //Storing data to open one single buffer at the end. Write to ram >> write to disk.
    std::list<std::pair<std::size_t,State>> traj;
    std::list<std::tuple<std::size_t,std::size_t, Obs>> record_obs;
    std::list<std::vector<std::size_t>> t_ep;
    //This vector has 4 entries because it stores the value and the theta of the 3 actions for each state, for each episode
    std::vector<std::vector<double>> value_policy(4*num_of_birds, std::vector<double>(
                                    std::max(state_space_dim,static_cast<std::size_t>(episodes_num*state_space_dim/1000))));
    
    //This stores the probability to be in each state at every moment for each bird
    std::vector<std::vector<double>> eta(state_space_dim, std::vector<double>(num_of_birds));
    std::size_t effective_num_of_steps = 0; //This is useful to compute the probability at the end

    //Output files of the simulation
    std::ofstream traj_file;
    std::ofstream obs_file;
    std::ofstream episode_file;
    std::ofstream eta_file;
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
    episode_file << "Episode,EndTime" << std::endl;

    for(std::size_t i=0; i<num_of_birds; ++i)
    {
     value_policy_files[i].open("data/value_policy"+std::to_string(i)+".csv");
     value_policy_files[i] << "value,";
     value_policy_files[i] << "left,";
     value_policy_files[i] << "straight,";
     value_policy_files[i] << "right";
     value_policy_files[i] << std::endl;
    }

    eta_file.open("data/eta.csv");
    for(std::size_t i=0; i<num_of_birds-1; ++i)
        eta_file << "eta_" + std::to_string(i) << ",";
    eta_file << "eta_" + std::to_string(num_of_birds-1) << std::endl;

    //---------------------------------------------------------------------------------

    //Time of episode
    std::size_t t = 0;
    
    //Here starts the actor-critic algorithm
    for(std::size_t ep=0; ep<episodes_num; ep++){

        //State initialization
        env.reset();
        prev_state = std::make_shared<State>(env.get_state());

        for(std::size_t i=0; i<agents.size(); ++i){
            prev_obs[i] = std::make_shared<Obs>(agents[i].obs(*prev_state));
        }
        
        //This determines whose turn is it to learn (is 0, pursuer, else, evaders)
        // ag_l = pred_training.step(ep);

        while(t < episode_length){

            //Data logging every 1000th episode
            if(ep%1000 == 0){
                traj.push_back(std::make_pair(ep,*prev_state));
                for(std::size_t i = 0; i<num_of_birds; ++i)
                    record_obs.push_back(std::make_tuple(ep,i,*prev_obs[i]));
            }

            //TODO: as of now this is meaningless
            //Update the number of times we visited state j for bird i; add an effective step
            // for(std::size_t i=0; i<num_of_birds; ++i){
            //     eta[(*prev_obs[i]).index()][i] += 1;
            // }
            // effective_num_of_steps++;
                
            //All agents get an observation based on the current state and return an action
            for(std::size_t i = 0; i < agents.size(); ++i){
                a[i] = (agents[i].act(*prev_state, *prev_obs[i]));
            }
            next_state = std::make_shared<State>(env.dynamics(a, *prev_state));
            
            #ifndef _OPENMP
                for(std::size_t i=0; i<agents.size(); ++i)
                    next_obs[i] = std::make_shared<Obs>(agents[i].obs(*next_state));
            #else
                #pragma omp parallel for
                for(std::size_t i=0; i<agents.size(); ++i)
                    next_obs[i] = std::make_shared<Obs>(agents[i].obs(*next_state));
            #endif
  
            //r contains the reward for all the agents on its first components 
            //and a boolean variable stating if the episode is finished or not in its second component
            r = env.reward(*prev_state, static_cast<double>(episode_length), num_of_birds-1);

            //If episode is over:
            if(std::get<1>(r) == 1){
                for(std::size_t i=0; i<num_of_birds; ++i){
                    //10-1 now we need the trace of the value vector to get td-error. 
                    //We also need to update every feature that is actually present
                    
                    // 19-1: we go back to full non-linear basis of functions
                    delta[i] = std::get<0>(r)[i] - v[i][*prev_obs[i]];
                    v[i][*prev_obs[i]] += alpha_w*delta[i];
                    // delta[i] = std::get<0>(r)[i] - scalar(v[i],*prev_obs[i]);
                    // for(std::size_t s=0; s<state_space_dim; ++s)
                    //     v[i][s] += alpha_w*delta[i]*((*prev_obs[i])[s]); //V values update
                }   
                //Theta values update
                for(std::size_t i=0;i<num_of_birds;++i)
                    agents[i].update_policy(alpha_t*delta[i], *prev_obs[i], a[i]);
                // if(ag_l != 0){ //Evaders are learning
                //     for(std::size_t j=1; j<num_of_birds; ++j)
                //         agents[j].update_policy(alpha_t*delta[j], *prev_obs[j], a[j]); 
                // } else{ //Pursuer is learning
                //     agents[0].update_policy(alpha_t*delta[0], *prev_obs[0], a[0]); 
                // }                  
                break;
            }
            
            for(std::size_t i=0; i<num_of_birds; ++i){
                
                // 19-1: we go back to full non-linear basis of functions
                delta[i] = std::get<0>(r)[i] + v[i][*next_obs[i]] - v[i][*prev_obs[i]];
                v[i][*prev_obs[i]] += alpha_w*delta[i];
                
                // delta[i] = std::get<0>(r)[i] + scalar(v[i], *next_obs[i]) - scalar(v[i],*prev_obs[i]);
                // for(std::size_t s=0; s<state_space_dim; ++s)
                //     v[i][s] += (*prev_obs[i])[s]*alpha_w*delta[i]; //V values update
            }     
         
            //Theta values updates
            for(std::size_t i=0;i<num_of_birds;++i)
                agents[i].update_policy(alpha_t*delta[i], *prev_obs[i], a[i]);
            // if(ag_l == 0){
            //     agents[ag_l].update_policy(alpha_t*delta[ag_l], *prev_obs[ag_l], a[ag_l]);
            // } else{
            //     for(std::size_t j=1; j<num_of_birds; ++j)
            //         agents[j].update_policy(alpha_t*delta[j], *prev_obs[j], a[j]);
            // }

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

        t_ep.push_back(std::vector<std::size_t>{ep,t});
        t = 0;
        
    } //End of an episode

    //Dividing by the number of number_time_steps to get actual probabilities
    for(std::size_t j=0; j<state_space_dim; ++j){
        for(std::size_t i=0; i<num_of_birds; ++i)
            eta[j][i] /= effective_num_of_steps;
    }

    //Transferring data to file at the end of everything
    for(auto it1=traj.begin(); it1!=traj.end(); ++it1){
        traj_file << std::get<0>(*it1) << "," << std::get<1>(*it1); //<< "\n";
    }

    for(auto it1=record_obs.begin(); it1!=record_obs.end(); ++it1){
        obs_file << std::get<0>(*it1) << "," << std::get<1>(*it1) << ","
                 << std::get<2>(*it1) << std::endl;
    }
        
    for(auto it=t_ep.begin(); it!=t_ep.end(); ++it)
        episode_file << (*it)[0] << "," << (*it)[1] << "\n";
  
    for(std::size_t i=0; i<num_of_birds; ++i){
        for(std::size_t k=0; k<value_policy[0].size(); ++k){
            value_policy_files[i] << value_policy[4*i][k] << ",";
            value_policy_files[i] << value_policy[4*i+1][k] << ",";
            value_policy_files[i] << value_policy[4*i+2][k] << ",";
            value_policy_files[i] << value_policy[4*i+3][k] << "\n";
        }
    }
    
    for(std::size_t i=0; i<state_space_dim; ++i){
        for(std::size_t j=0; j<num_of_birds-1; ++j)
            eta_file << eta[i][j] << ",";
        eta_file << eta[i][num_of_birds-1] << "\n";
    }

    //Closing files
    traj_file.close();
    episode_file.close();
    for(std::size_t i=0; i<num_of_birds; ++i)
        value_policy_files[i].close();
    obs_file.close();
    eta_file.close();

    return 0;
}

