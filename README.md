# DTNSIM #

DTNSIM is a simulator devoted to study different aspects of Delay/Disruption Tolerant Network (DTN) such as routing, forwarding, scheduling, planning, and others. 

The simulator is implemented on the [Omnet++ framework version 5.5.1](https://omnetpp.org/) and can be conveniently utilized using the Qtenv environment and modified or extended using the Omnet++ Eclipse-based IDE. Check [this video in youtube](https://youtu.be/_5HhfNULjtk) for a quick overview of the tool.

The simulator is still under development: this is a beta version. Nonetheless, feel free to use it and test it. Our contact information is below. 

## Installation ##

* Download [Omnet++](https://omnetpp.org/omnetpp). DTNSIM was tested on version 5.5.1.
* Import the DTNSIM repository from the Omnet++ IDE (File->Import->Projects from Git->Clone URI).
* Build DTNSIM project.

* Open dtnsim_demo.ini config file from the use cases folder.
* Run the simulation. If using Omnet++ IDE, the Qtenv environment will be started. 
* Results (scalar and vectorial statistics) will be stored in the results folder.

Note: Nodes will remain static in the simulation visualization. Indeed, the dynamic of the network is captured in the "contact plans" which comprises a list of all time-bound communication opportunities between nodes. In other words, if simulating mobile network, the mobility should be captured in such contact plans and provided to DTNSIM as input.

## ION Support ##

Interplanetary Overlay Network (ION) flight code is supported in the support-ion branch. Currenly, ION 3.5.0 Contact Graph Routing library is supported by DTNSIM.

## Contact Us ##

If you have any comment, suggestion, or contribution you can reach us at madoerypablo@gmail.com and juanfraire@gmail.com.