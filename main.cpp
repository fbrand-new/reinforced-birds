#include "State.h"
#include "Observable.h"
#include "Agent.h"
#include "Environment.h"
#include "Reward.h"
#include <memory>
#include <algorithm>

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

    //Decide the episode length
    std::size_t episode_length = 10;

    //Decide the number of birds. 
    //Each of them is an agent, some of them will just use a fixed policy
    std::size_t num_of_birds = 3;

    //The first one is the pursuer, the other are evaders

    //State initialization
    Environment env(num_of_birds); //How many birds
    std::vector<Agent> agents(num_of_birds);

    for(std::size_t i = 0; i < agents.size(); ++i ){
        agents[i].set_id(i);
    }
    //Environment env(... velocity, max angle, etc...)

    std::shared_ptr<State> prev_state;
    std::shared_ptr<State> next_state;
    std::vector<Action> a;
    std::vector<Reward> r;

    std::ofstream traj_file;
    traj_file.open("first_trajectory.csv");
    traj_file << "x,y" << std::endl;

    //State initialization
    prev_state = std::make_shared<State>(env.get_state());

    std::cout << "Bird initialized in: " << prev_state << std::endl;

    //We should also loop on multiple episodes
    for(std::size_t t = 0; t<episode_length; ++t){
        traj_file << prev_state;
        
        //All agents get an observation based on the current state and return an action
        for(std::size_t i = 0; i < agents.size(); ++i)
            a[i] = (agents[i].act(*prev_state));
            
        next_state = std::make_shared<State>(env.dynamics(a, *prev_state));
        //r = std::make_unique<Reward>(env.reward(*a, *prev_state));

        //Learning phase
        //TODO: for TD learning
        //TODO: for Monte-Carlo, we should actually put in memory a whole episode each time

        prev_state = next_state;
    }

    return 0;
}

// ----------------------------------------------------------------------

// Function definition

// Action random_walk(){
//     return static_cast<Action>( rand() % (static_cast<int>(Action::right)+1));
// }

