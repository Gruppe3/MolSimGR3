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
	int idx[] = { p.getX()[0] / sim->cellsSize[0], p.getX()[1] / sim->cellsSize[1], p.getX()[2] / sim->cellsSize[2] };
	utils::Vector<double, 3>& f = p.getF();
	if (!sim->membrane) {
		f[1] = f[1] + sim->gravity * p.getM();
	} else {
		//LOG4CXX_DEBUG(forcelog, "idx for gravity:  "<<idx[0]<<"  "<<idx[1]<<"   "<<idx[2]);
		f[2] = f[2] + sim->gravity * p.getM();
		int i = 0; int j = 1;
		while (i < sim->pulledUpParticlesLength) {
			//LOG4CXX_DEBUG(forcelog, "gravity upwards    i:   "<<i<<"   j:   "<<j);
			if ((sim->pulledUpParticles[i] == idx[0]) && (sim->pulledUpParticles[j] == idx[1])) {
				f[2] = f[2] + sim->gravityUpwards * p.getM();
			}
			i += 2;	j += 2;
		}
	}
}
void EarthGravitation::calc(Particle& p1, Particle& p2) {
}
