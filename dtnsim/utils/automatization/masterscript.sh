#!/bin/bash

helpFunction()
{
   echo ""
   echo "Usage: $0 -a parameterA"
   echo -e "\t-a Number of total simulations"
   exit 1 # Exit script after printing help
}

while getopts "a:" opt
do
   case "$opt" in
      a ) parameterA="$OPTARG" ;;
      ? ) helpFunction ;; # Print helpFunction in case parameter is non-existent
   esac
done

# Print helpFunction in case parameters are empty
if [ -z "$parameterA" ]
then
   echo "Some or all of the parameters are empty";
   helpFunction
fi

# Begin script in case all parameters are correct

rx=200 #max traffic the simulation can generate (kb) 
rt=200 #max time for simulation intervals (h)         
for s in $(seq 1 $parameterA)
do
    t=$(shuf -i1-$rt -n1)
    x=$(shuf -i1-$rx -n1)
    let T=1800 #$t*60 #min (60) 
    let X=$x*1024 #how many KB  
    bash typescript.sh -a $T -b $X -c $s
done
