#!/bin/bash

# This is used by the CMake benchmarking steps.

satid=$1
sat=$2
echo Running benchmark for satid $satid / sat $sat

echo $satid | /data/starfs1/compilers/idl_8.7/idl87/bin/idl -e ".run benchmark.pro"

tar cjf benchmark_${sat}.tar.bz2 ${sat}*.png

