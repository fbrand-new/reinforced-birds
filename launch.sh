#!/bin/bash
#SBATCH -p long # queue
#SBATCH -N 1 # n nodes
#SBATCH -n 20 # n cores
#SBATCH -o birds.txt # STDOUT
#SBATCH -J NACBirds
#SBATCH -v

dt=$(date +"%H:%M:%S - %d/%m/%y")
echo "Job started @ ${dt}"

./main.x

dt=$(date +"%H:%M:%S - %d/%m/%y")
echo "Job finished @ ${dt}"