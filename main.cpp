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

// Main execution
int main(){

    //Environment params
    constexpr std::size_t action_num = 3;
    double steering_angle = M_PI/6;
    double capture_range = 0.5;
    constexpr double gamma = 1;

    //Decide the number of birds. 
    //Each of them is an agent, some of them will just use a fixed policy
    std::size_t num_of_birds = 10;
    constexpr std::size_t sectors_num = 5;
    const std::size_t state_space_dim = pow(2, sectors_num);
    //The first one is the pursuer, the other are evaders

    //Learning rates
    double alpha_w = 0.01;
    double alpha_t = 0.0001;

    //Decide the episode length
    std::size_t episodes_num = 60000;
    std::size_t episode_length = 500;

    //Instantiate a learning signal to alternate between preys and predator learning
    std::vector<std::size_t> learning_agent;
    for (auto i=1; i<static_cast<int>(episodes_num/2000); ++i){
        learning_agent.push_back(2000*i);
    }
    Signal pred_training(learning_agent);

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
    //Reward r(num_of_birds);
    std::vector<V> v(num_of_birds, V(state_space_dim));
    std::vector<double> delta(num_of_birds);
    std::size_t ag_l; //The agent currently learning
    std::pair<Reward,bool> r; //Reward 

    //Matrix storing data
    std::list<std::pair<std::size_t,State>> traj;
    std::list<std::vector<std::size_t>> t_ep;
    Eigen::MatrixXd value_policy(episodes_num*state_space_dim, 4*num_of_birds);

    //Output files of the simulation
    std::ofstream traj_file;
    std::ofstream episode_file;
    std::ofstream value_policy_file;

    //File initialization
    traj_file.open("data/pursuer_trajectory.csv");

    //Header construction
    traj_file << "Episode" << ",";
    for(std::size_t i=0; i<num_of_birds; ++i){
        traj_file << "x" << i << ",y" << i;
        if(i<num_of_birds-1) 
            traj_file << ",";
    }
    traj_file << "\n";

    episode_file.open("data/episode.csv");
    episode_file << "Episode,EndTime,PredatorTraining" << std::endl;
    value_policy_file.open("data/value_policy.csv");
    for(auto i=0; i<num_of_birds-1; i++){
        value_policy_file << "value_" + std::to_string(i) << ",";
        value_policy_file << "left_" + std::to_string(i) << ",";
        value_policy_file << "straight_" + std::to_string(i) << ",";
        value_policy_file << "right_" + std::to_string(i) << ",";
    }

    value_policy_file << "value_" + std::to_string(num_of_birds-1) << ",";
    value_policy_file << "left_" + std::to_string(num_of_birds-1) << ",";
    value_policy_file << "straight_" + std::to_string(num_of_birds-1) << ",";
    value_policy_file << "right_" + std::to_string(num_of_birds-1) << std::endl;
    // value_policy_file << 
    //                 "Value_p,Left_p,Straight_p,Right_p,Value_e,Left_e,Straight_e,Right_e" 
    //                 << std::endl;

    //Time of episode
    std::size_t t = 0;

    //We should also loop on multiple episodes
    for(std::size_t ep=0; ep<episodes_num; ep++){

        //State initialization
        env.reset();
        prev_state = std::make_shared<State>(env.get_state());
        for(std::size_t i=0; i<num_of_birds; ++i)
            prev_obs[i] = std::make_shared<Observable>(agents[i].obs(*prev_state));
      
        ag_l = pred_training.multistep(ep,num_of_birds);

        while(t < episode_length){
            
            //traj_file << *prev_state;
            if(ep%100 == 0)
                traj.push_back(std::make_pair(ep,*prev_state));

            //All agents get an observation based on the current state and return an action
            for(std::size_t i = 0; i < agents.size(); ++i){
                a[i] = (agents[i].act(*prev_state, *prev_obs[i]));
            }
    
            next_state = std::make_shared<State>(env.dynamics(a, *prev_state));
            for(std::size_t i=0; i<num_of_birds; ++i)
                next_obs[i] = std::make_shared<Observable>(agents[i].obs(*next_state));

            r = env.reward(*prev_state);

            //Check if episode is over:
            if(std::get<1>(r) == 1){
                for(std::size_t i=0; i<num_of_birds; ++i){
                    delta[i] = std::get<0>(r)[i] - v[i][*prev_obs[i]];
                    v[i][*prev_obs[i]] += alpha_w*delta[i]; //V values update
                }   
                //Theta values update
                agents[ag_l].update_policy(alpha_t*delta[ag_l], *prev_obs[ag_l], a[ag_l]);
                // if(pred_learning==0){
                //     agents[0].update_policy(alpha_t*delta[0], *prev_obs[0], a[0]);
                // } else {
                //     agents[1].update_policy(alpha_t*delta[1], *prev_obs[1], a[1]);
                // }   
                break; 
            }

            for(std::size_t i=0; i<num_of_birds; ++i){
                delta[i] = std::get<0>(r)[i] + gamma*v[i][*next_obs[i]] - v[i][*prev_obs[i]];
                v[i][*prev_obs[i]] += alpha_w*delta[i]; //V values update
            }              

            //Theta values update
            agents[ag_l].update_policy(alpha_t*delta[ag_l], *prev_obs[ag_l], a[ag_l]);
            // if(pred_learning==0){
            //     agents[0].update_policy(alpha_t*delta[0], *prev_obs[0], a[0]);
            // } else {
            //     agents[1].update_policy(alpha_t*delta[1], *prev_obs[1], a[1]);
            // }

            //State update
            prev_state = next_state;

            //Observation update
            for(std::size_t i=0; i<num_of_birds; ++i)
                prev_obs[i] = next_obs[i];

            //Update time step
            t++;
        }

        for(std::size_t i=0; i < num_of_birds; ++i){
            for(std::size_t k=0; k<state_space_dim; ++k){
                value_policy(ep*state_space_dim+k, i*4) = v[i][k];
                value_policy(ep*state_space_dim+k, i*4+1) = agents[i].get_policy()->get(k,0);
                value_policy(ep*state_space_dim+k, i*4+2) = agents[i].get_policy()->get(k,1);
                value_policy(ep*state_space_dim+k, i*4+3) = agents[i].get_policy()->get(k,2);
            }
        }
        
        t_ep.push_back(std::vector<std::size_t>{ep,t,ag_l});
        t = 0;
    }

    for(auto it1=traj.begin(); it1!=traj.end(); ++it1)
        traj_file << std::get<0>(*it1) << "," << std::get<1>(*it1); //<< "\n";

    for(auto it=t_ep.begin(); it!=t_ep.end(); ++it)
        episode_file << (*it)[0] << "," << (*it)[1] << "," << (*it)[2] << "\n";

    for(auto i=0; i<episodes_num*state_space_dim; ++i){
        for(auto j=0; j<4*num_of_birds-1; ++j)
            value_policy_file << value_policy(i,j) << ",";
        value_policy_file << value_policy(i,4*num_of_birds-1) << "\n";
    }

    traj_file.close();
    episode_file.close();
    value_policy_file.close();
    return 0;
}

