/*
 * harmonicPotential.cpp
 *
 *  Created on: 27.12.2014
 *      Author: vmuser
 */

#include <forces/HarmonicPotential.h>
#include "Logger.h"

const LoggerPtr forcelog(log4cxx::Logger::getLogger("molsim.particle.force"));

HarmonicPotential::HarmonicPotential(Simulation* s) {
	setSimulation(s);
}

HarmonicPotential::~HarmonicPotential() {

}

void HarmonicPotential::calc(Particle& p1, Particle& p2) {
	int idx = 0;
	while (&p2 != p1.Neighbour[idx]){
		idx++;
	}
	//if (sim->cutoff > pow(2, 1/6)){
		double r0;
		if (idx % 2 == 0)
			r0 = sim->harmonicR0 * sqrt(2);	// diagonal neighbor
		else
			r0 = sim->harmonicR0;
		utils::Vector<double, 3>& f1 = p1.getF();
		utils::Vector<double, 3> diff = p2.getX() - p1.getX();
		//LOG4CXX_DEBUG(forcelog, "X1: "<<p1.getX().toString()<<"  X2: "<<p2.getX().toString());
		double diff_norm = diff.L2Norm();
		utils::Vector<double, 3> f = sim->stiffnessConstant * (diff_norm - r0) / diff_norm * diff;
		f1 = f1 + f;
	//}
	/*else {
		LennardJonesLC* ljForce=new LennardJonesLC(sim);
		ljForce->calc(p1,p2) ;
	}*/
}
