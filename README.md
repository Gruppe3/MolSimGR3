MolSim
===

The Molecular Dynamics teaching code.

##Build Instructions

1. Make sure to have installed the libraries libxerces-c, libcppunit, liblog4cxx on your system.
2. Use the provided makefile.

##Run Instructions

The following parameter configurations are possible:

1. **MolSim -test testcase**	This runs a test suite from /src/test/. The parameter should be the name of one of the test cases without the "Test", eg.: "XMLInputTest" --> ./MolSim -test XMLInput
2. **MolSim -xml filename**	This runs the simulation based on data stored in the specified XML file (e.g. eingabe-two_bodies.xml). See molsim-input.xsd for valid XML definitions. The XML file must include a correct reference path to the XSD file!
3. **MolSim -m filename**		Like -xml, but uses the special calculation method from the task "Simulation of a membrane". It is assumed that only cuboids are to be created, and that they only extend in the XY-plane, with only one particle in the Z axis; just as described in the task.
4. **MolSim -c filename [delta_t end_time]**	This runs the simulation based on cuboids generated from data in the specified file (e.g. eingabe-cuboid.txt).
5. **MolSim -p filename [delta_t end_time]**	This runs the simulation based on particles stored in the specified file (e.g. eingabe-sonne.txt).

**[delta_t end_time]** is optional to define values for time step and end time of simulation (default values: 0.014 and 1000)

A Doxygen documentation of the code can be found in directory html.