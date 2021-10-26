#include "State.h"
#include "Observable.h"
#include "Agent.h"
#include "Environment.h"
#include "Reward.h"
#include "Policy.h"
#include "RandomWalk.h"
#include "Boltzmann.h"
#include "V.h"
#include "Signal.h"
#include <string>
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

Executions:
- Random walk
*/

//---------------------------------
// Hardcoded policies

// random walk policy
// Action random_walk();

// ----------------------------------- ---

// Main execution
int main(){

    //Environment params
    constexpr std::size_t action_num = 3;
    double steering_angle = M_PI/6;
    double capture_range = 0.5;
    constexpr double gamma = 1;

    //Decide the number of birds. 
    //Each of them is an agent, some of them will just use a fixed policy
    std::size_t num_of_birds = 2;
    constexpr std::size_t sectors_num = 5;
    const std::size_t state_space_dim = pow(2, sectors_num);
    //The first one is the pursuer, the other are evaders

    //Learning rates
    double alpha_w = 0.001;
    double alpha_t = 0.0001;

    //Decide the episode length
    std::size_t episodes_num = 5000;
    std::size_t episode_length = 500;

    //Instantiate a learning signal to alternate between preys and predator learning
    Signal pred_training(std::vector<std::size_t>{1000, 2000, 5000});

    //---------------------------------------------------------------------------------

    //State initialization
    Environment env(num_of_birds, 0.1, capture_range, steering_angle); //How many birds

    //Agents initialization
    std::vector<Agent> agents(num_of_birds);
    for(std::size_t i = 0; i < agents.size(); ++i ){
        agents[i].set_id(i);
        agents[i].set_policy<Boltzmann>(Boltzmann(state_space_dim, 3));
    }
    
    //Bunch of pointers to keep track of value during the run and avoiding hard copying
    std::shared_ptr<State> prev_state;
    std::shared_ptr<State> next_state;
    std::vector<Action> a(num_of_birds);
    std::vector<std::shared_ptr<Observable>> prev_obs(num_of_birds);
    std::vector<std::shared_ptr<Observable>> next_obs(num_of_birds);
    Reward r(num_of_birds);
    std::vector<V> v(num_of_birds, V(state_space_dim));
    std::vector<double> delta(num_of_birds);

    //Output files of the simulation
    std::ofstream traj_file;
    std::ofstream episode_file;
    std::ofstream policy_pursuer_file;
    std::ofstream policy_evader_file;
    std::ofstream value_pursuer_file;
    std::ofstream value_evader_file;

    episode_file.open("episode.csv");
    episode_file << "Episode,EndTime,PredatorTraining" << std::endl;
    policy_pursuer_file.open("policy_pursuer.csv");
    policy_pursuer_file << "Episode,State,Left,Straight,Right" << std::endl;
    policy_evader_file.open("policy_evader.csv");
    policy_evader_file << "Episode,State,Left,Straight,Right" << std::endl;
    value_pursuer_file.open("value_pursuer.csv");
    value_pursuer_file << "Episode,State,Value" << std::endl;
    value_evader_file.open("value_pursuer.csv");
    value_evader_file << "Episode,State,Value" << std::endl;

    //Time of episode
    std::size_t t = 0;

    //We should also loop on multiple episodes
    for(std::size_t ep=0; ep<episodes_num; ep++){

        //State initialization
        env.reset();
        prev_state = std::make_shared<State>(env.get_state());
        for(std::size_t i=0; i<num_of_birds; ++i)
            prev_obs[i] = std::make_shared<Observable>(agents[i].obs(*prev_state));
      
        //File initialization
        traj_file.open("trajectories/pursuer_trajectory" + std::to_string(ep) + ".csv");

        //Header construction
        for(std::size_t i=0; i<num_of_birds; ++i){
            traj_file << "x" << i << ",y" << i;
            if(i<num_of_birds-1) 
                traj_file << ",";
        }
        traj_file << std::endl;

        bool pred_learning = pred_training.step(ep);

        while(t < episode_length){
            traj_file << *prev_state;
            
            //All agents get an observation based on the current state and return an action
            for(std::size_t i = 0; i < agents.size(); ++i){
                a[i] = (agents[i].act(*prev_state, *prev_obs[i]));
            }
    
            next_state = std::make_shared<State>(env.dynamics(a, *prev_state));
            for(std::size_t i=0; i<num_of_birds; ++i)
                next_obs[i] = std::make_shared<Observable>(agents[i].obs(*next_state));

            r = env.reward(*prev_state);

            //Check if episode is over:
            if(r[0] == 1){
                break;
            }

            for(std::size_t i=0; i<num_of_birds; ++i){
                delta[i] = r[i] + gamma*v[i][*next_obs[i]] - v[i][*prev_obs[i]];
                v[i][*prev_obs[i]] += alpha_w*delta[i]; //V values update
            }              

            //Theta values update
            if(pred_learning){
                agents[0].update_policy(alpha_t*delta[0], *prev_obs[0], a[0]);
            } else {
                agents[1].update_policy(alpha_t*delta[1], *prev_obs[1], a[1]);
            }

            //State update
            prev_state = next_state;

            //Observation update
            for(std::size_t i=0; i<num_of_birds; ++i)
                prev_obs[i] = next_obs[i];

            //Update time step
            t++;
        }

        //File update
        //The lower the number the further right on the vision cone we are
        for(std::size_t s=0; s<state_space_dim; ++s){
            value_pursuer_file << ep << "," << s << "," << v[0][s] << std::endl;
            value_evader_file << ep << "," << s << "," << v[1][s] << std::endl;
            policy_pursuer_file  << ep << "," << s << ",";
            policy_evader_file  << ep << "," << s << ",";
            for(std::size_t ac=0; ac<action_num-1; ++ac){
                policy_pursuer_file << agents[0].get_policy()->get(s,ac) << ",";
                policy_evader_file << agents[1].get_policy()->get(s,ac) << ",";
            }
            policy_pursuer_file << agents[0].get_policy()->get(s, action_num-1) << std::endl;
            policy_evader_file << agents[1].get_policy()->get(s, action_num-1) << std::endl;
        }
        
        traj_file.close();
        episode_file << ep << "," << t << "," << pred_learning << std::endl;
        t = 0;
    }

    episode_file.close();
    value_evader_file.close();
    value_pursuer_file.close();
    policy_evader_file.close();
    policy_pursuer_file.close();
    return 0;
}

