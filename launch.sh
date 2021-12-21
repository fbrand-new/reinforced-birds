#!/bin/bash
#SBATCH -p serial # queue
#SBATCH -N 1 # n nodes
#SBATCH -n 1 # n cores
#SBATCH -o out.txt # STDOUT
#SBATCH -J firstTry
#SBATCH -v

dt=$(date +"%H:%M:%S - %d/%m/%y")
echo "Job started @ ${dt}"

./main.x

dt=$(date +"%H:%M:%S - %d/%m/%y")
echo "Job finished @ ${dt}"