#!/bin/bash

helpFunction()
{
   echo ""
   echo "Usage: $0 -a parameterA -b parameterB -c parameterC"
   echo -e "\t-a Time intervals"
   echo -e "\t-b Amount of data generated"
   echo -e "\t-c Number of simulations"
   exit 1 # Exit script after printing help
}

while getopts "a:b:c:" opt
do
   case "$opt" in
      a ) parameterA="$OPTARG" ;;
      b ) parameterB="$OPTARG" ;;
      c ) parameterC="$OPTARG" ;;
      ? ) helpFunction ;; # Print helpFunction in case parameter is non-existent
   esac
done

# Print helpFunction in case parameters are empty
if [ -z "$parameterA" ] || [ -z "$parameterB" ] || [ -z "$parameterC" ]
then
   echo "Some or all of the parameters are empty";
   helpFunction
fi

# Begin script in case all parameters are correct

#generate .ini
cd "/home/omn/git/dtnsim/simulations/dtnsim_demo"
python script.py $parameterA $parameterB $parameterC

#run the simulation
cd "/home/omn/git/dtnsim"
./dtnsim -f "simulations/dtnsim_demo/traffic$parameterC.ini" -u Cmdenv

#generate .csv (vectors and scalars)
cd "/home/omn/Documents/omnetpp-5.6.2/bin"
./scavetool x "/home/omn/git/dtnsim/simulations/dtnsim_demo/results/General-#0.vec" -o "/home/omn/Documents/handlingData/data/vec$parameterC.csv"
./scavetool x "/home/omn/git/dtnsim/simulations/dtnsim_demo/results/General-#0.sca" -o "/home/omn/Documents/handlingData/data/sca$parameterC.csv"

#generate vec dicts
cd "/home/omn/Documents/handlingData"
python vec_dict_script.py "/home/omn/Documents/handlingData/data/vec$parameterC.csv" $parameterC
python sca_dict_script.py "/home/omn/Documents/handlingData/data/sca$parameterC.csv" $parameterC

#generate plots
#python plot_vec_script.py $parameterC

