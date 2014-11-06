
#include <forces/ForceHandler.h>
#include <forces/Gravitation.h>
#include <forces/LennardJones.h>
#include "MaxwellBoltzmannDistribution.h"
#include "ParticleContainer.h"
#include "ParticleGenerator.h"
#include "io/ParticlesInput.h"
#include "io/VTKOutput.h"
#include "io/XYZOutput.h"

#include "test/Runner.cpp"
#include "test/LennardJonesTest.h"
#include "test/ParticleContainerTest.h"
#include "test/ParticleGeneratorTest.h"

#include "Logger.h"

#include <list>
#include <cstring>
#include <cstdlib>
#include <iostream>

using namespace std;

/**** forward declaration of the calculation functions ****/

/**
 * calculate the force between particles
 */
void calculateF(ForceHandler& force);

/**
 * calculate the position for all particles
 */
void calculateX();

/**
 * calculate the position for all particles
 */
void calculateV();

/**
 * plot the particles to a xyz-file
 */
void plotParticles(int iteration);

/**
 * log parameter error */
void error();

extern const LoggerPtr molsimlog;

double start_time = 0;
double end_time = 1000;
double delta_t = 0.014;

ParticleContainer particleContainer;


int main(int argc, char* argsv[]) {
	PropertyConfigurator::configure("log4cxx.conf");
	LOG4CXX_INFO(molsimlog, "Hello from MolSim for PSE!");
	switch (argc) {
		case 2: 	// single file or test case
			if (strcmp(argsv[1], "-test") == 0) {
				run(LennardJonesTest::suite());
				return 0;
			}
			else {
				error();
				return 1;
			}
		case 3:	// cuboid or particle data
			break;
		case 5:
			delta_t = atof(argsv[3]);
			end_time = atof(argsv[4]);// different delta_t and endtime
			break;
		default:
			error();
			return 1;
	}

	// get input data
	if (strcmp(argsv[1], "-c") == 0) {	// cuboids
		ParticleGenerator pg;
		pg.getFileInput(argsv[2], particleContainer);
	}
	else if (strcmp(argsv[1], "-p") == 0) {	// particles
		ParticlesInput pi;
		pi.getFileInput(argsv[2], particleContainer);
	}
	else {
		error();
		return 1;
	}

	LOG4CXX_INFO(molsimlog, "Starting calculation...");
	// the forces are needed to calculate x, but are not given in the input file.
	//Gravitation forceType;
	LennardJones forceType;
	calculateF(forceType);

	double current_time = start_time;

	int iteration = 0;

	 // for this loop, we assume: current x, current f and current v are known
	while (current_time < end_time) {
		// calculate new x
		calculateX();

		// calculate new f
		calculateF(forceType);

		// calculate new v
		calculateV();

		iteration++;
		if (iteration % 10 == 0) {
			plotParticles(iteration);
			LOG4CXX_DEBUG(molsimlog, "Iteration " << iteration << " finished.");
		}
		//LOG4CXX_DEBUG(logger, "Iteration " << iteration << " finished.");

		current_time += delta_t;
	}

	LOG4CXX_INFO(molsimlog, "output written. Terminating...");
	return 0;
}

void calculateF(ForceHandler& force) {
	// initialize forces of all particles with 0 for this time step
	particleContainer.resetIterator();
	while (particleContainer.hasNext()) {
		Particle& p = particleContainer.next();
		utils::Vector<double, 3>& f = p.getF();
		utils::Vector<double, 3>& f_old = p.getOldF();
		f_old = f;
		f = 0.0;
	}
	particleContainer.resetIterator();

	// calculate new forces
	while (particleContainer.hasNext()) {
		Particle& p1 = particleContainer.next();
		while (particleContainer.hasNextOther()) {
			Particle& p2 = particleContainer.nextOther();
			force.calc(p1, p2);
		}
	}
}


void calculateX() {
	particleContainer.resetIterator();

	while (particleContainer.hasNext()) {
		Particle& p = particleContainer.next();

		// insert calculation of X here!
		utils::Vector<double, 3>& x = p.getX();
		x = x + delta_t * (p.getV() + delta_t / (2 * p.getM()) * p.getF());
	}
}


void calculateV() {
	particleContainer.resetIterator();

	while (particleContainer.hasNext()) {
		Particle& p = particleContainer.next();

		// insert calculation of velocity here!
		utils::Vector<double, 3>& v = p.getV();
		v = v + delta_t / (2 * p.getM()) * (p.getOldF() + p.getF());
	}
}


void plotParticles(int iteration) {
	string out_name("MD_vtk");

	VTKOutput vtko;
	vtko.setOutput(out_name, iteration, particleContainer);
}

void error() {
	LOG4CXX_ERROR(molsimlog, "Errounous programme call!");
	LOG4CXX_ERROR(molsimlog, "./MolSim (-c | -p) filename [delta_t end_time] | -test");
}
