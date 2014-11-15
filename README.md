MolSim
===

The Molecular Dynamics teaching code.

##Build Instructions

1. Make sure to have installed the libraries libxerces-c, libcppunit, liblog4cxx on your system
2. Use the provided makefile

##Run Instructions

The following parameter configurations are possible:

1. MolSim -test	This runs a test suite defined in MolSim.cpp
2. MolSim -c filename [delta_t end_time]	This runs the simulation based on cuboids generated from data in the specified file
3. MolSim -p filename [delta_t end_time]	This runs the simulation based on particles stored in the specified file

[delta_t end_time] is optional to define values for time step and end time of simulation (default values: 0.014 and 1000)

A Doxygen documentation of the code can be found in directory html.