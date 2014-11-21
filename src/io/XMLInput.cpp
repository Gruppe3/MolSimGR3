/*
 * XMLInput.cpp
 *
 *  Created on: 15.11.2014
 *      Author: christiansturm
 */

#include <io/input.h>
#include "XMLInput.h"
#include "../Logger.h"
#include "../ParticleGenerator.h"

using namespace std;
using namespace input;

extern double start_time;
extern double end_time;
extern double delta_t;
extern string out_name;
extern int writeFreq;
extern double delta_t;
extern double domainSize[3];
extern double cutoff;
extern const LoggerPtr iolog;

XMLInput::~XMLInput() {
	// TODO Auto-generated destructor stub
}

void XMLInput::getFileInput(char* fileName, ParticleContainer* pc) {
	try {
		// initialize xml object
		string file(fileName);
		auto_ptr<molsimdata> molsim (molsimdata_ (file));

		// set simulation parameters
		LOG4CXX_INFO(iolog, "reading simulation parameters...");
		start_time = 0;
		out_name = molsim->outputbasename();
		writeFreq = molsim->writefreq();
		delta_t = molsim->timestep();
		end_time = molsim->endtime();
		domainSize[0] = molsim->domain()->size().x();
		domainSize[1] = molsim->domain()->size().y();
		domainSize[2] = molsim->domain()->size().z();
		cutoff = molsim->domain()->cutoff();
		LOG4CXX_DEBUG(iolog, "out: " << out_name << ", wFq: " << writeFreq << ", delta t: " <<
				delta_t << ", end: " << end_time << ", cutoff: " << cutoff << ", domain: " <<
				domainSize[0] << " x " << domainSize[1] << " x " << domainSize[2]);

		LOG4CXX_INFO(iolog, "reading object data...");
		objectlist objects = molsim->objectlist();

		// iterating over cuboids
		for (objectlist::cuboid_const_iterator i (objects.cuboid().begin());
	         i != objects.cuboid().end();
	         ++i) {
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

	    	ParticleGenerator pg;
	    	pg.createCuboid(x, n, v, i->meshwidth(), i->mass(), i->brownian(), pc);
	    }

		// iterating over spheres
		for (objectlist::sphere_const_iterator i (objects.sphere().begin());
			 i != objects.sphere().end();
			 ++i) {
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

			ParticleGenerator pg;
			pg.createSphere(x, i->numparticles(), v, i->meshwidth(), 1.0, 0.1, pc);
		}

		// iterating over particles
		for (objectlist::particle_const_iterator i (objects.particle().begin());
			 i != objects.particle().end();
			 ++i) {
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

			Particle p(x, v, i->mass(), 0);
			pc->add(p);
		}
		LOG4CXX_INFO(iolog, "reading done...");
	}
	catch (const xml_schema::exception& e) {
		LOG4CXX_FATAL(iolog, "couldn't read XML file: " << e);
		exit(-1);
	}
}
