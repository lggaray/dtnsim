DTNSIM Plotting Tool

In this folder there are python and bash scripts which allow 
to generate pdf files from the .sca and .vec omnet result files
The .vec and .sca result files must be generated in sqlite format 
by setting the following configuration in the .ini file:
outputvectormanager-class="omnetpp::envir::SqliteOutputVectorManager"
outputscalarmanager-class="omnetpp::envir::SqliteOutputScalarManager"

In order to generate the pdf files the following steps must be performed:

1.- run an omnet simulation. Omnet result files will be saved in the folder results
2.- if necessary, give execution permissions to bash.sh script from a terminal console: chmod +x bash.sh
3.- execute bash script from a terminal console: ./bash.sh

pdf files are generated in results folder also.

Modifications to files location can be done in the bash script, while modifications to the
plots can be done in the python script.

Note:
The following python libraries are needed: 
matplotlib, math 


