/*
 * EarthGravitation.cpp
 *
 *  Created on: 07.12.2014
 *      Author: vmuser
 */

#include <forces/EarthGravitation.h>
#include "Logger.h"

const LoggerPtr forcelog(log4cxx::Logger::getLogger("molsim.particle.force"));

EarthGravitation::EarthGravitation(Simulation *s) {
	sim = s;
}

EarthGravitation::~EarthGravitation() {

}

void EarthGravitation::iterateFunc(Particle& p) {
	utils::Vector<double, 3>& f = p.getF();
	if (sim->membrane) {
		f[2] = f[2] + sim->gravity * p.getM();

		if (sim->current_timestep > sim->forceUpwardsEnd)
			return;

		int idx[] = { p.getX()[0] / sim->cellsSize[0],
			p.getX()[1] / sim->cellsSize[1],
			p.getX()[2] / sim->cellsSize[2] };
		//LOG4CXX_DEBUG(forcelog, "idx for gravity:  "<<idx[0]<<"  "<<idx[1]<<"   "<<idx[2]);
		int i = 0; int j = 1;
		while (i < sim->pulledUpParticlesLength) {
			//LOG4CXX_DEBUG(forcelog, "gravity upwards    i:   "<<i<<"   j:   "<<j);
			if ((sim->pulledUpParticles[i] == idx[0]) && (sim->pulledUpParticles[j] == idx[1])) {
				//LOG4CXX_DEBUG(forcelog, "gravity upwards    i:   "<<i<<"   j:   "<<j);
				f[2] = f[2] + sim->forceUpwards;
				//LOG4CXX_DEBUG(forcelog, "p:" << p.getX().toString());
				/*for (int n = 0; n < 8; n++) {
					if (p.Neighbour[n] != NULL) {
						LOG4CXX_DEBUG(forcelog, "neig:" << p.Neighbour[n]->getX().toString());
					}

				}*/
				//LOG4CXX_DEBUG(forcelog, "f[2]: "<< f[2]);
			}
			i += 2;	j += 2;
		}
	} else if (sim->typeflag) {
		if (sim->states[p.getType()] == "liquid") {
			f[1] = f[1] + sim->gravity * p.getM();
		}
	} else {
		f[1] = f[1] + sim->gravity * p.getM();
	}
}
void EarthGravitation::calc(Particle& p1, Particle& p2) {
}
