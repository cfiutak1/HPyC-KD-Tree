#!/bin/bash
#SBATCH --job-name=chuntering_from_a_sedentary_position
#SBATCH --cpus-per-task=24
#SBATCH -N 1
#SBATCH --mem=8GB

python generate_training_file.py 5000000 5 0
python generate_query_file.py 1 5 0 3
