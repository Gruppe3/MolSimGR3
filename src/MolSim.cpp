
#include <forces/ForceHandler.h>
#include <forces/Gravitation.h>
#include <forces/LennardJones.h>
#include <forces/HarmonicPotential.h>
#include <ParticleContainerLC.h>
#include "PhaseSpaceOutput.h"
#include "XAxisProfileOutput.h"
#include "forces/EarthGravitation.h"
#include "MaxwellBoltzmannDistribution.h"
#include "ParticleContainer.h"
#include "ParticleGenerator.h"
#include "DynamicsCalc.h"
#include "io/ParticlesInput.h"
#include "io/XMLInput.h"
#include "io/VTKOutput.h"
#include "io/XYZOutput.h"
#include "Simulation.h"

#include "test/Runner.cpp"
#include "test/LennardJonesTest.h"
#include "test/ParticleContainerTest.h"
#include "test/ParticleContainerLCTest.h"
#include "test/ParticleGeneratorTest.h"
#include "test/XMLInputTest.h"
#include "test/ThermostatTest.h"

#include "Logger.h"

#include <list>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sys/time.h>

using namespace log4cxx;
using namespace log4cxx::helpers;
using namespace std;

// undefine LC to use ParticleContainer without LC algo
#define LC


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

// init logger
const LoggerPtr molsimlog(Logger::getLogger("molsim"));
const LoggerPtr particlelog(Logger::getLogger("molsim.particle"));
const LoggerPtr forcelog(log4cxx::Logger::getLogger("molsim.particle.force"));
const LoggerPtr testlog(log4cxx::Logger::getLogger("molsim.test"));
const LoggerPtr iolog(log4cxx::Logger::getLogger("molsim.io"));

/** the container that will be used to store particles */
ParticleContainer* particleContainer;
/** stores simulation parameters */
Simulation *sim;


//particle generator should work, but there is Speicherabzugfehler in iterateDirectNeighbours in ParticleContainerLC

int main(int argc, char* argsv[]) {
	PropertyConfigurator::configure("log4cxx.conf");
	LOG4CXX_INFO(molsimlog, "Hello from MolSim for PSE!");
	sim = new Simulation();

	switch (argc) {
		case 2: 	// single file
			error();
			return 1;
		case 3:		// xml, cuboid or particle file, or test case
			if (strcmp(argsv[1], "-test") == 0) {
				if (strcmp(argsv[2], "LennardJones") == 0) {run(LennardJonesTest::suite());}
				if (strcmp(argsv[2], "ParticleContainer") == 0) {run(ParticleContainerTest::suite());}
				if (strcmp(argsv[2], "ParticleContainerLC") == 0) {run(ParticleContainerLCTest::suite());}
				if (strcmp(argsv[2], "ParticleGenerator") == 0) {run(ParticleGeneratorTest::suite());}
				if (strcmp(argsv[2], "XMLInput") == 0) {run(XMLInputTest::suite());}
				if (strcmp(argsv[2], "Thermostat") == 0) {run(ThermostatTest::suite());}
				return 0;
			}
			else {
				break;
			}
		case 5:
			sim->delta_t = atof(argsv[3]);
			sim->end_time = atof(argsv[4]);// different delta_t and endtime
			break;
		default:
			error();
			return 1;
	}

	// get input data
	particleContainer = new ParticleContainer;
	//Gravitation forceType;
	ForceHandler* forceType = new LennardJones();

	CalcX *xcalc = new CalcX(sim);
	CalcV *vcalc = new CalcV(sim);
	CalcT *tcalc = new CalcT(sim);
	EarthGravitation* gravity = new EarthGravitation(sim);
	HarmonicPotential* hpotential = new HarmonicPotential(sim);

	XAxisProfile* xprofile = new XAxisProfile();
	InputHandler* inputhandler;
	if (strcmp(argsv[1], "-c") == 0) {	// cuboids
		inputhandler = new ParticleGenerator;
	}
	else if (strcmp(argsv[1], "-p") == 0) {	// particles
		inputhandler = new ParticlesInput;
	}
	else if (strcmp(argsv[1], "-xml") == 0) {	// xml file according to molsim-input.xsd
		inputhandler = new XMLInput;
		#ifdef LC
		forceType = new LennardJonesLC;
		#endif
	}
	else if (strcmp(argsv[1], "-m") == 0) {	// xml file according to molsim-input.xsd + calculation as in the task "Simulation of a membrane"
		inputhandler = new XMLInput;
		(*sim).membrane = true;
		#ifdef LC
		forceType = new LennardJonesLC;
		#endif
	}
	else {
		error();
		return 1;
	}

	inputhandler->getFileInput(argsv[2], particleContainer, sim);
	//delete inputhandler;

	if ((strcmp(argsv[1], "-xml") == 0)|| strcmp(argsv[1], "-m") == 0) {
		#ifdef LC
		particleContainer = new ParticleContainerLC(particleContainer, sim);
		LOG4CXX_DEBUG(molsimlog, "init ParticleContainerLC");
		#endif
	}

	forceType->setSimulation(sim);
	hpotential->setSimulation(sim);
	xprofile->setValues(sim->numOfBins, sim->domainSize);

	LOG4CXX_INFO(molsimlog, "Starting calculation...");

	// the forces are needed to calculate x, but are not given in the input file.
	// copies F to oldF of particles and sets F to 0
	particleContainer->iterate(forceType);
	//LOG4CXX_DEBUG(molsimlog, "after f to 0");

	#ifdef LC
	((ParticleContainerLC*)particleContainer)->applyBoundaryConds(BoundaryConds::PERIODIC, forceType);
	#endif
	// calculates new F
	particleContainer->iteratePair(forceType);
	if (sim->membrane) {
		((ParticleContainerLC*)particleContainer)->iterateDirectNeighbours(hpotential);
		//LOG4CXX_DEBUG(molsimlog, "after neighbors");
	}
	//LOG4CXX_DEBUG(molsimlog, "before gravity");
	particleContainer->iterate(gravity);
	#ifdef LC
	// add reflecting force to boundary particles according to domainBoundaries[]
	((ParticleContainerLC*)particleContainer)->applyBoundaryConds(BoundaryConds::REFLECTING, forceType);
	((ParticleContainerLC*)particleContainer)->emptyHalo();
	#endif

	double current_time = sim->start_time;
	int iteration = 0;
	int count_iterations = sim->end_time / sim->delta_t;
	double beta = 1.0;
	 // for this loop, we assume: current x, current f and current v are known
	double temperature = sim->initTemp;	
	/**
 	* set up the timeval structures for performance measurement
 	*/
	struct timeval performance_timer_start;
	struct timeval performance_timer_current;

	/**
 	* Get the starting time for performance measurement & declare double variables.
 	*/
	gettimeofday(&performance_timer_start, NULL);
	double performance_timer_start_double = (double)performance_timer_start.tv_sec + (double)performance_timer_start.tv_usec / 1000000.0;
	double performance_timer_current_double = 0.0;
	double performance_timer_average = 0.0;

	while (current_time < sim->end_time) {
		// calculate new x
		//LOG4CXX_DEBUG(molsimlog, "before x");
		particleContainer->iterate(xcalc);
		//LOG4CXX_DEBUG(molsimlog, "after x");
		#ifdef LC
		((ParticleContainerLC*)particleContainer)->moveParticles();
		//LOG4CXX_DEBUG(molsimlog, "after move");
		((ParticleContainerLC*)particleContainer)->applyBoundaryConds(BoundaryConds::PERIODIC, forceType);
		//LOG4CXX_DEBUG(molsimlog, "after periodic");
		#endif

		// copies F to oldF of particles and sets F to 0
		particleContainer->iterate(forceType);
		// calculate new f
		particleContainer->iteratePair(forceType);
		if(sim->membrane){
			particleContainer->iterateDirectNeighbours(hpotential);
		}
		particleContainer->iterate(gravity);
		//LOG4CXX_DEBUG(molsimlog, "after forces");

		#ifdef LC
		// add reflecting force to boundary particles according to sim->domainBoundaries[]
		((ParticleContainerLC*)particleContainer)->applyBoundaryConds(BoundaryConds::REFLECTING, forceType);
		//LOG4CXX_DEBUG(molsimlog, "after reflecting");
		#endif

		if (sim->tempFreq > 0) {
			vcalc->setBeta(1.0);
			tcalc->resetEnergy();
			if ((iteration > sim->thermostatStart) && (iteration % sim->tempFreq == 0)){
				tcalc->resetEnergy();
				//calculates sum to get T
				particleContainer->iterate(tcalc);
				if (temperature < sim->targetTemp){
					temperature += sim->tempDiff;	// heating
				}
				else if (temperature > sim->targetTemp) {
					temperature -= sim->tempDiff;	// cooling
				}
				beta = tcalc->calcBeta(temperature, particleContainer->size());
				//LOG4CXX_DEBUG("molsim",beta<<)
				vcalc->setBeta(beta);
			}
		}
		/*particleContainer->resetIterator();
		while(particleContainer->hasNext()){
			cout<<particleContainer->next().getV().toString()<<endl;
		}*/

		// calculate new v
		particleContainer->iterate(vcalc);
		//LOG4CXX_DEBUG(molsimlog, "after v");

		iteration++;
		if (iteration % sim->writeFreq == 0) {
			/**
 			* Get the elapsed time by subtracting the start time from the current time. Then, divide it by the number of elapsed iterations.
 			*/
			gettimeofday(&performance_timer_current, NULL);
			performance_timer_current_double = (double)performance_timer_current.tv_sec + (double)performance_timer_current.tv_usec / 1000000.0;
			performance_timer_average = (performance_timer_current_double - performance_timer_start_double) / (double)iteration;

			plotParticles(iteration);
			LOG4CXX_DEBUG(molsimlog, "Iteration " << iteration << " of " << count_iterations << " finished. Avg: " << performance_timer_average << " sec./Iter.");
			//LOG4CXX_DEBUG(molsimlog, "Size ot Particles : " << particleContainer->size() );
			//LOG4CXX_DEBUG(molsimlog,"beta"<<beta);
		}


#ifdef LC
		// remove halo particles
		((ParticleContainerLC*)particleContainer)->emptyHalo();
		//LOG4CXX_DEBUG(molsimlog, "after empty");
		//((ParticleContainerLC*)particleContainer)->applyBoundaryConds(BoundaryConds::OUTFLOW, forceType);
#endif

		current_time += sim->delta_t;

		//writes mean velocity and density of bins in XAxisProfileOutput files
		if (sim->typeflag){
			if((iteration-1)%sim->writeFreqFlowSim==0){
				xprofile->openFile(iteration);
				//xprofile->setValues(sim->numOfBins,sim->domainSize);
			}

			particleContainer->iterate(xprofile);
			//xprofile->setValues(sim->numOfBins,sim->domainSize);
			xprofile->writeInFile();
			if(iteration%sim->writeFreqFlowSim==0){
				xprofile->closeFile();
			}
		}
	}
	//makes sure file is closed even if iterations were interrupted
	xprofile->closeFile();

	LOG4CXX_INFO(molsimlog,"Starting PhaseSpaceOutput...");
	PSO *pso = new PSO();
	pso->openFile(particleContainer->size());
	particleContainer->iterate(pso);
	pso->closeFile();
	LOG4CXX_INFO(molsimlog,"PhaseSpaceOutput finished.");



	LOG4CXX_INFO(molsimlog, "Output written. Terminating...");
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
		x = x + sim->delta_t * (p.getV() + sim->delta_t / (2 * p.getM()) * p.getF());
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
		v = v + sim->delta_t / (2 * p.getM()) * (p.getOldF() + p.getF());
	//}
}


void plotParticles(int iteration) {
	if (sim->out_name.empty())
		return;

	VTKOutput vtko;
	vtko.setOutput(sim->out_name, iteration, particleContainer);
}

void error() {
	LOG4CXX_ERROR(molsimlog, "Errounous programme call!");
	LOG4CXX_ERROR(molsimlog, "./MolSim (-c | -p) filename [delta_t end_time] | -xml filename | -test name of tested item");
}
