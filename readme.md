## Reinforced birds

This code was developed for my master thesis in Data Science and Scientific computing at the university of Trieste.

Using MultiAgent reinforcement learning, we seek to solve a pursuit-evasion problem between a chasing bird (pursuer) and some fleeing birds (evaders). In particular, we are interested in any non-trivial collective behaviour emerging spontaneously from the simulation. 

The thesis work was done at ICTP, working together with professor Antonio Celani and with Emanuele Panizon.

## How to use this code

Easy:

    git clone https://github.com/earendil94/reinforced-evasion.git
    cd ./reinforced-evasion
    make parallel
    ./main.x

This will launch a training with 10 birds with the default configuration defined in the main.cpp file.

Tweak the numbers in config.h to change parameters and hyperparameters of the system. 



## Post code analysis

`AnimateTrajectory` and `EpisodePlotting` are two python notebooks used for analyzing data produced by the simulation. The former produces animation of the trajectories created by the simulation, while the second one plots episode duration and localized averaged velocities of the flock. 