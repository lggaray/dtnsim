### DTNSIM experiments automatization

In this folder there are bash scripts which allow to run batch of expriments

* masterscript.sh:  as its name suggests, this script runs t times simulations.
Inside the file, you can configure rx (max traffic the simulation can generate) and rt (max time for the simulation intervals)
Input: 
-a Number of simulations to run (t)

* typescript.sh: this is the core script. 
-> First, generate the configuration files (.ini)
 -> Second, run the simulation
  -> Third, generate the .csv with scavetool (dtnsim tool)
   -> Fourth, generate the dicts
   input: 
   -a Time intervals
   -b Amount of data generated
   -c Number of simulations

Note:
Pay attention to all the paths and directories
