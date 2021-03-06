/*
 * XMLInput.cpp
 *
 *  Created on: 15.11.2014
 *      Author: christiansturm
 */

#include <io/input.h>
#include "XMLInput.h"
#include "../FileReader.h"
#include "../Logger.h"
#include "../ParticleGenerator.h"

using namespace std;
using namespace input;

const LoggerPtr iolog(log4cxx::Logger::getLogger("molsim.io"));

XMLInput::~XMLInput() {
	// TODO Auto-generated destructor stub
}

void XMLInput::getFileInput(char* fileName, ParticleContainer* pc,
		Simulation *sim) {
	try {
		// initialize xml object
		string file(fileName);
		auto_ptr<molsimdata> molsim(molsimdata_(file));

		// set simulation parameters
		LOG4CXX_INFO(iolog, "reading simulation parameters...");
		sim->start_time = 0;
		sim->out_name = molsim->outputbasename();
		sim->writeFreq = molsim->writefreq();
		sim->delta_t = molsim->timestep();
		sim->end_time = molsim->endtime();
		if (molsim->force().present())
			sim->forceType = molsim->force().get();
		sim->gravity = molsim->gravity();
		sim->domainSize[0] = molsim->domain()->size().x();
		sim->domainSize[1] = molsim->domain()->size().y();
		sim->domainSize[2] = molsim->domain()->size().z();
		if (sim->domainSize[2] == 0)
			sim->domainSize[2] = 1;
		sim->cutoff = molsim->domain()->cutoff();
		if (molsim->domain()->bins().present()) {
			sim->numOfBins = molsim->domain()->bins().get();
		}
		if (molsim->domain()->membrane().present()) {
			sim->membrane = true;
			sim->harmonicR0 = molsim->domain()->membrane()->harmonicR0();
			sim->stiffnessConstant =
					molsim->domain()->membrane()->stiffnessConstant();
			sim->forceUpwards =
					molsim->domain()->membrane()->constForceUpwards();
		} else {
			sim->membrane = false;
		}
		LOG4CXX_DEBUG(iolog,
				"out: " << sim->out_name << ", wFq: " << sim->writeFreq << ", delta t: " << sim->delta_t << ", end: " << sim->end_time << ", cutoff: " << sim->cutoff << ", gravity:" << sim->gravity << ", domain: " << sim->domainSize[0] << " x " << sim->domainSize[1] << " x " << sim->domainSize[2] << ", numOfBins: " << sim->numOfBins);

		// get boundary conditions
		BoundaryConds *bc = sim->boundaries;
		string str = molsim->domain()->boundaries().front();
		bc->setBoundary(BoundaryConds::FRONT, defineBoundary(str));
		str = molsim->domain()->boundaries().back();
		bc->setBoundary(BoundaryConds::BACK, defineBoundary(str));
		str = molsim->domain()->boundaries().left();
		bc->setBoundary(BoundaryConds::LEFT, defineBoundary(str));
		str = molsim->domain()->boundaries().right();
		bc->setBoundary(BoundaryConds::RIGHT, defineBoundary(str));
		str = molsim->domain()->boundaries().top();
		bc->setBoundary(BoundaryConds::TOP, defineBoundary(str));
		str = molsim->domain()->boundaries().bottom();
		bc->setBoundary(BoundaryConds::BOTTOM, defineBoundary(str));

		// set thermostat values
		double brownian = 0.0;
		thermostat thermo = molsim->thermostat();
		if (thermo.brownian().present()) {
			brownian = thermo.brownian().get();
			sim->meanVelocityTypeFlag = 0;
		} else
			sim->meanVelocityTypeFlag = 1;
		sim->initTemp = thermo.inittemp();
		sim->targetTemp = thermo.targettemp();
		sim->tempDiff = thermo.tempdiff();
		sim->tempFreq = thermo.interval();
		sim->thermostatStart = thermo.starttime();

		int typeSize = molsim->particleTypes().type().size();
		sim->typesNum = typeSize;
		sim->epsilons = new double[typeSize];
		sim->sigmas = new double[typeSize];
		sim->states = new string[typeSize];

		// set sigma and epsilon values for given types
		for (int i = 0; i < molsim->particleTypes().type().size(); i++) {
			sim->epsilons[i] = molsim->particleTypes().type()[i].epsilon();
			sim->sigmas[i] = molsim->particleTypes().type()[i].sigma();
			if (molsim->particleTypes().type()[i].state().present()) {
				sim->typeflag = true;
				sim->states[i] = molsim->particleTypes().type()[i].state().get();
				//LOG4CXX_DEBUG(iolog, " particle state "<< i << ": "<<sim->states[i]);
			} else {
				sim->typeflag = false;
				sim->states[i] = "none";
			}
		}

		LOG4CXX_INFO(iolog, "reading object data...");
		objectlist objects = molsim->objectlist();

		// iterating over cuboids
		for (objectlist::cuboid_const_iterator i(objects.cuboid().begin());
				i != objects.cuboid().end(); ++i) {
			LOG4CXX_DEBUG(iolog, "generating cuboid...");
			// set coordinates
			utils::Vector<double, 3> x;
			x[0] = i->location().x();
			x[1] = i->location().y();
			x[2] = i->location().z();

			// set dimensions
			utils::Vector<int, 3> n;
			n[0] = i->numparticles().x();
			n[1] = i->numparticles().y();
			n[2] = i->numparticles().z();

			// set velocity
			utils::Vector<double, 3> v;
			v[0] = i->velocity().x();
			v[1] = i->velocity().y();
			v[2] = i->velocity().z();

			sim->meshWidth = i->meshwidth();
			ParticleGenerator pg;
			int type = 0;
			//sim->epsilon11 = molsim->particleTypes().type()[0].epsilon();

			//sim->sigma11=molsim->particleTypes().type()[0].sigma();
			if (i->type().present()) {
				type = i->type().get();
				if (type >= typeSize) {
					LOG4CXX_ERROR(iolog, "type " << type << "doesn't exist");
					exit(-1);
				}
				/*sim->epsilon11 = molsim->particleTypes().type()[0].epsilon();
				 sim->epsilon22 = molsim->particleTypes().type()[1].epsilon();
				 sim->sigma11 = molsim->particleTypes().type()[0].sigma();
				 sim->sigma22 = molsim->particleTypes().type()[1].sigma();*/
				/*LOG4CXX_DEBUG(iolog, "epsilon11:  "<< sim->epsilon11 << "  epsilon22:  "<< sim->epsilon22
				 << "  sigma11:  "<<sim->sigma11 << "  sigma22:  "<<sim->sigma22);*/
				/*sim->sigma12=(sim->sigma11+sim->sigma22)/2;
				 sim->epsilon12=sqrt(sim->epsilon11*sim->epsilon22);*/
			}
			pg.createCuboid(x, n, v, i->meshwidth(), i->mass(), brownian, pc,
					sim, type);
		}

		// iterating over spheres
		for (objectlist::sphere_const_iterator i(objects.sphere().begin());
				i != objects.sphere().end(); ++i) {
			LOG4CXX_DEBUG(iolog, "generating sphere...");
			// set coordinates
			utils::Vector<double, 3> x;
			x[0] = i->location().x();
			x[1] = i->location().y();
			x[2] = i->location().z();

			// set velocity
			utils::Vector<double, 3> v;
			v[0] = i->velocity().x();
			v[1] = i->velocity().y();
			v[2] = i->velocity().z();

			sim->meshWidth = i->meshwidth();
			ParticleGenerator pg;
			int type = 0;
			if (i->type().present()) {
				type = i->type().get();
				if (type >= typeSize) {
					LOG4CXX_ERROR(iolog, "type " << type << "doesn't exist");
					exit(-1);
				}
			}
			pg.createSphere(x, i->numparticles(), v, i->meshwidth(), i->mass(),
					brownian, 2, pc, sim, type);
		}

		// iterating over particles
		for (objectlist::particle_const_iterator i(objects.particle().begin());
				i != objects.particle().end(); ++i) {
			LOG4CXX_DEBUG(iolog, "generating particle...");
			// set coordinates
			utils::Vector<double, 3> x;
			x[0] = i->location().x();
			x[1] = i->location().y();
			x[2] = i->location().z();

			// set velocity
			utils::Vector<double, 3> v;
			v[0] = i->velocity().x();
			v[1] = i->velocity().y();
			v[2] = i->velocity().z();

			int type = 0;
			if (i->type().present()) {
				type = i->type().get();
				if (type >= typeSize) {
					LOG4CXX_ERROR(iolog, "type " << type << "doesn't exist");
					exit(-1);
				}
			}
			Particle p(x, v, i->mass(), 0);
			pc->add(p);
		}

		if (molsim->objectlist().inputfiles().present()) {
			LOG4CXX_INFO(iolog, "reading further specified input files...");
			input::inputfiles ipf = molsim->objectlist().inputfiles().get();
			FileReader *fr = new FileReader;
			for (int i = 0; i < ipf.particles().size(); i++) {
				fr->readFile(pc, (char*) ipf.particles()[i].c_str());
			}
		}

		LOG4CXX_INFO(iolog, "reading done...");
	} catch (const xml_schema::exception& e) {
		LOG4CXX_FATAL(iolog, "couldn't read XML file: " << e);
		exit(-1);
	}
}

BoundaryConds::Boundary XMLInput::defineBoundary(string str) {
	return string("outflow").compare(str) == 0 ?
			BoundaryConds::OUTFLOW :
			(string("reflecting").compare(str) == 0 ?
					BoundaryConds::REFLECTING : BoundaryConds::PERIODIC);
}
