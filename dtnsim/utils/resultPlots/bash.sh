#!/bin/bash

# delete old generated files
rm -rf ../results/dtnsim.*.pdf
rm -rf ../results/dtnsim.*.pdf
rm -rf ../results/dtnsim.*.pdf

#generate new pdf files from .vec and .sca files
python3 mainVec.py ../results ../results
python3 mainSca.py ../results ../results
