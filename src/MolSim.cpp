
#include <forces/ForceHandler.h>
#include <forces/Gravitation.h>
#include <forces/LennardJones.h>
#include <ParticleContainerLC.h>
#include "MaxwellBoltzmannDistribution.h"
#include "ParticleContainer.h"
#include "ParticleGenerator.h"
#include "DynamicsCalc.h"
#include "io/ParticlesInput.h"
#include "io/XMLInput.h"
#include "io/VTKOutput.h"
#include "io/XYZOutput.h"

#include "test/Runner.cpp"
#include "test/LennardJonesTest.h"
#include "test/ParticleContainerTest.h"
#include "test/ParticleGeneratorTest.h"
#include "test/XMLInputTest.h"

#include "Logger.h"

#include <list>
#include <cstring>
#include <cstdlib>
#include <iostream>

using namespace std;

/**** forward declaration of the calculation functions ****/

/**
 * calculate the force between particles
 * @param force Type of force to calculate (Gravitation, LennardJones)
 */
void calculateF(ForceHandler* force);

/**
 * calculate the position for all particles
 * @param LC Flag for usage of LC algorithm
 */
void calculateX(Particle& p);

/**
 * calculate the position for all particles
 */
void calculateV(Particle& p);

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
string out_name("MD_vtk");
int writeFreq = 10;
/** domain size for LC algo */
double domainSize[3];
/** cut off radius for LC algo */
double cutoff;
/** distance between particles (2^(1/6)*sigma) */
double meshWidth;

/** stores boundary conditions in this order: left, right, bottom, top, front, back */
Boundary domainBoundaries[6];

/** the container that will be used to store particles */
ParticleContainer* particleContainer;

int main(int argc, char* argsv[]) {
	PropertyConfigurator::configure("log4cxx.conf");
	LOG4CXX_INFO(molsimlog, "Hello from MolSim for PSE!");
	switch (argc) {
		case 2: 	// single file
			error();
			return 1;
		case 3:		// xml, cuboid or particle file, or test case
			if (strcmp(argsv[1], "-test") == 0) {
				if (strcmp(argsv[2], "LennardJones") == 0) {run(LennardJonesTest::suite());}
				if (strcmp(argsv[2], "ParticleContainer") == 0) {run(ParticleContainerTest::suite());}
				if (strcmp(argsv[2], "ParticleGenerator") == 0) {run(ParticleGeneratorTest::suite());}
				if (strcmp(argsv[2], "XMLInput") == 0) {run(XMLInputTest::suite());}
				return 0;
			}
			else {
				break;
			}
		case 5:
			delta_t = atof(argsv[3]);
			end_time = atof(argsv[4]);// different delta_t and endtime
			break;
		default:
			error();
			return 1;
	}

	// get input data
	particleContainer = new ParticleContainer;
	//Gravitation forceType;
	ForceHandler* forceType = new LennardJones;
	CalcX *xcalc = new CalcX;
	CalcV *vcalc = new CalcV;
	InputHandler* inputhandler;
	if (strcmp(argsv[1], "-c") == 0) {	// cuboids
		inputhandler = new ParticleGenerator;
	}
	else if (strcmp(argsv[1], "-p") == 0) {	// particles
		inputhandler = new ParticlesInput;
	}
	else if (strcmp(argsv[1], "-xml") == 0) {	// xml file according to io/input.xsd
		inputhandler = new XMLInput;
		// delete #define to use ParticleContainer without LC algo
		#define LC
#ifdef LC
		forceType = new LennardJonesLC;
#endif
	}
	else {
		error();
		return 1;
	}

	inputhandler->getFileInput(argsv[2], particleContainer);
	delete inputhandler;

	if (strcmp(argsv[1], "-xml") == 0) {
#ifdef LC
		/*ParticleContainerLC* pc = new ParticleContainerLC(cutoff, meshWidth, domainSize,
				domainBoundaries, particleContainer);*/
		particleContainer = new ParticleContainerLC(cutoff, meshWidth, domainSize,
				domainBoundaries, particleContainer);
		LOG4CXX_DEBUG(molsimlog, "init ParticleContainerLC");
#endif
	}

	LOG4CXX_INFO(molsimlog, "Starting calculation...");
	// the forces are needed to calculate x, but are not given in the input file.

	// copies F to oldF of particles and sets F to 0
	particleContainer->iterate(forceType);
	// calculates new F
	particleContainer->iteratePair(forceType);
#ifdef LC
	// add reflecting force to boundary particles according to domainBoundaries[]
	((ParticleContainerLC*)particleContainer)->applyBoundaryConds(REFLECTING, forceType);
#endif

	double current_time = start_time;

	int iteration = 0;
	int count_iterations = end_time/delta_t;

	 // for this loop, we assume: current x, current f and current v are known
	while (current_time < end_time) {
		// calculate new x
		particleContainer->iterate(xcalc);
#ifdef LC
		((ParticleContainerLC*)particleContainer)->moveParticles();//LOG4CXX_DEBUG(molsimlog, "after move");
#endif

		// copies F to oldF of particles and sets F to 0
		particleContainer->iterate(forceType);
		// calculate new f
		particleContainer->iteratePair(forceType);
#ifdef LC
		// add reflecting force to boundary particles according to domainBoundaries[]
		((ParticleContainerLC*)particleContainer)->applyBoundaryConds(REFLECTING, forceType);
#endif

		// calculate new v
		particleContainer->iterate(vcalc);


		iteration++;
		if (iteration % writeFreq == 0) {
			plotParticles(iteration);
			LOG4CXX_DEBUG(molsimlog, "Iteration " << iteration << " of " << count_iterations << " finished.");
		}


#ifdef LC
		// remove halo particles
		((ParticleContainerLC*)particleContainer)->emptyHalo();
#endif
		current_time += delta_t;
	}

	LOG4CXX_INFO(molsimlog, "output written. Terminating...");
	return 0;
}

void calculateF(ForceHandler* force) {
	// initialize forces of all particles with 0 for this time step

	particleContainer->resetIterator();
	while (particleContainer->hasNext()) {
		//LOG4CXX_DEBUG(molsimlog, "calc f before next1");
		Particle& p = particleContainer->next();
		utils::Vector<double, 3>& f = p.getF();
		utils::Vector<double, 3>& f_old = p.getOldF();
		f_old = f;
		f = 0.0;
	}
	particleContainer->resetIterator();

	// calculate new forces
	while (particleContainer->hasNext()) {
		//LOG4CXX_DEBUG(molsimlog, "calc f before next2");
		Particle& p1 = particleContainer->next();
		while (particleContainer->hasNextOther()) {
			Particle& p2 = particleContainer->nextOther();
			//LOG4CXX_DEBUG(molsimlog, "f1 before :" << p1.getF().toString());
			//LOG4CXX_DEBUG(molsimlog, "x1:" << p1.getX().toString() << ", x2:" << p2.getX().toString());
			force->calc(p1, p2);
			//LOG4CXX_DEBUG(molsimlog, "f1 after :" << p1.getF().toString());
		}
	}
}


void calculateX(Particle& p) {
	/*particleContainer->resetIterator();
//int count = 0;
	while (particleContainer->hasNext()) {
		Particle& p = particleContainer->next();
		//LOG4CXX_DEBUG(molsimlog, "calc x of:" << p.getX().toString());
		// insert calculation of X here!
		*/
		utils::Vector<double, 3>& x = p.getX();
		x = x + delta_t * (p.getV() + delta_t / (2 * p.getM()) * p.getF());
		//LOG4CXX_DEBUG(molsimlog, "new x:" << x.toString());
		//count++;
	//}
	//LOG4CXX_DEBUG(molsimlog, "count x:" << count);
}


void calculateV(Particle& p) {
	/*particleContainer->resetIterator();

	while (particleContainer->hasNext()) {
		Particle& p = particleContainer->next();*/

		// insert calculation of velocity here!
		utils::Vector<double, 3>& v = p.getV();
		v = v + delta_t / (2 * p.getM()) * (p.getOldF() + p.getF());
	//}
}


void plotParticles(int iteration) {
	//string out_name("MD_vtk");

	VTKOutput vtko;
	vtko.setOutput(out_name, iteration, particleContainer);
}

void error() {
	LOG4CXX_ERROR(molsimlog, "Errounous programme call!");
	LOG4CXX_ERROR(molsimlog, "./MolSim (-c | -p) filename [delta_t end_time] | -xml filename | -test");
}
