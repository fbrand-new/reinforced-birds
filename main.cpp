#include "State.h"
#include "Observable.h"
#include "Agent.h"
#include "Environment.h"
#include "Reward.h"
#include "Policy.h"
#include "RandomWalk.h"
#include "V.h"
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

    double capture_range = 0.05;
    constexpr double gamma = 1;
    double alpha_w = 0.01;
    double alpha_t = 0.01;
    //Decide the episode length
    std::size_t episode_length = 100;

    //Decide the number of birds. 
    //Each of them is an agent, some of them will just use a fixed policy
    std::size_t num_of_birds = 2;
    constexpr std::size_t sectors_num = 5;
    const std::size_t state_space_dim = pow(3, sectors_num);
    //The first one is the pursuer, the other are evaders

    //State initialization
    Environment env(num_of_birds,0.2,capture_range); //How many birds
    std::vector<Agent> agents(num_of_birds);


    //3 is the fixed number of action that we can take
    //TODO: The Q matrix corresponds to the thetas of the policy though
    // Eigen::MatrixXd Q(state_space_dim, 3);

    // //Q matrix initialization
    // for(std::size_t i=0; i<Q.rows(); ++i){
    //     for(std::size_t j=0; j<Q.cols(); ++j)
    //         Q(i,j) = rand()/RAND_MAX;
    // }

    //We will actually need a collection of V, one for each agent
    //Eigen::VectorXd V(state_space_dim);  

    for(std::size_t i = 0; i < agents.size(); ++i ){
        agents[i].set_id(i);
        agents[i].set_policy<RandomWalk>(RandomWalk(1));
    }
    
    std::shared_ptr<State> prev_state;
    std::shared_ptr<State> next_state;
    std::vector<Action> a(num_of_birds);
    std::vector<std::shared_ptr<Observable>> prev_obs(num_of_birds);
    std::vector<std::shared_ptr<Observable>> next_obs(num_of_birds);
    Reward r(num_of_birds);
    std::vector<V> v(num_of_birds, V(state_space_dim));
    std::vector<double> delta(num_of_birds);

    std::ofstream traj_file;
    traj_file.open("first_trajectory.csv");

    //Header construction
    for(std::size_t i=0; i<num_of_birds; ++i){
        traj_file << "x" << i << ",y" << i;
        if(i<num_of_birds-1) 
            traj_file << ",";
    }
    traj_file << std::endl;


    //State initialization
    prev_state = std::make_shared<State>(env.get_state());
    for(std::size_t i=0; i<num_of_birds; ++i)
        prev_obs[i] = std::make_shared<Observable>(agents[i].obs(*prev_state));

    std::cout << "Bird initialized in: " << *prev_state << std::endl;

    //We should also loop on multiple episodes
    for(std::size_t t = 0; t<episode_length; ++t){
        traj_file << *prev_state;
        
        //All agents get an observation based on the current state and return an action
        for(std::size_t i = 0; i < agents.size(); ++i){
            a[i] = (agents[i].act(*prev_state, *prev_obs[i]));
        }
 
        next_state = std::make_shared<State>(env.dynamics(a, *prev_state));
        for(std::size_t i=0; i<num_of_birds; ++i)
            next_obs[i] = std::make_shared<Observable>(agents[i].obs(*next_state));

        r = env.reward(*prev_state);

        for(std::size_t i=0; i<num_of_birds; ++i)
            delta[i] = r[i] + gamma*v[i][*next_obs[i]] - v[i][*prev_obs[i]];

        //Learning phase
        //V values update
        for(std::size_t i=0; i<num_of_birds; ++i)
            v[i][*prev_obs[i]] += alpha_w*delta[i];

        //Theta values update (here comes the tricky)
        for(std::size_t i=0; i<num_of_birds; ++i)


        //Check if episode is over:
        if(r[0] == 1){
            std::cout << "Episode finished at time " << t <<  std::endl;
            break;
        }




        //State update
        prev_state = next_state;

        //Observation update
        for(std::size_t i=0; i<num_of_birds; ++i)
            prev_obs[i] = next_obs[i];
    }

    std::cout << "Episode finished at time " << episode_length <<  std::endl;

    return 0;
}

// ----------------------------------------------------------------------

// Function definition

// Action random_walk(){
//     return static_cast<Action>( rand() % (static_cast<int>(Action::right)+1));
// }

