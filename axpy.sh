#!/bin/bash
#SBATCH --partition=Orion
#SBATCH --job-name=axpy_pthread
#SBATCH --output=axpy_%j.out
#SBATCH --error=axpy_%j.err
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --time=00:30:00
#SBATCH --mem=64G

# Compile
make clean
make


for t in 1 2 4 8 16 32 36; do
  ./axpy 1000000 $t
done

for t in 1 2 4 8 16 32 36; do
  ./axpy 10000000 $t
done

for t in 1 2 4 8 16 32 36; do
  ./axpy 100000000 $t
done

for t in 1 2 4 8 16 32 36; do
  ./axpy 1000000000 $t
done