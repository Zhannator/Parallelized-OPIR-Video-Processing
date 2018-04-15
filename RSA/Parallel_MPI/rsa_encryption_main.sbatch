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

srun -n 2 $SLURM_SUBMIT_DIR/rsa_encryption_main 1000 $SLURM_SUBMIT_DIR/Cloud001_1000.raw 381.47

crc-job-stats.py

#######
# Guide
#
# 0. This script assumes that you run 'sbatch example.sbatch' with your programs
#    stored in ./demo
#
# 1. When compiling your applications, be sure to use the same version of the
#    tools as the ones you'll use on the cluster. Otherwise, your 'hello'
#    application won't work. (some demos use C99 extensions)
#
#      user$ module purge
#      user$ module load gcc/5.4.0 openmpi/3.0.0
#      user$ mpicc demo/hello.c -o demo/hello -std=c99
#
# 2. Running your application. A log file 'slurm-$JOBID' will be created showing
#    the stderr/stdout of your application and the CRC statistics.
#
#      user$ sbatch example.sbatch
#      user$ less slurm-$JOBID
#
#    Use 'squeue -M mpi -u $USER' to see the status of your jobs.
#
# 3. This script also shows how to copy files to/from the MPI cluster.
#    Replace 'INPUT' with actual input file(s) and 'OUTPUT' with actual
#    output file(s), if any.
