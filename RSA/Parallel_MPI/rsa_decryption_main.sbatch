#!/bin/bash
#SBATCH --job-name=ztest
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=1
#SBATCH --cluster=mpi
#SBATCH --mail-user=zhp3@pitt.edu
#SBATCH --mail-type=END,FAIL
#SBATCH --time=0-00:10:00
#SBATCH --qos=short

module purge
module load gcc/5.4.0 openmpi/3.0.0

###############################
# Place your command line below

srun -n 2 $SLURM_SUBMIT_DIR/rsa_decryption_main 1000 $SLURM_SUBMIT_DIR/encrypted.raw 381.47

crc-job-stats.py