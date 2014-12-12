/*
 * LennardJones.cpp
 *
 *  Created on: 02.11.2014
 *      Author: christiansturm
 */

#include "LennardJones.h"
#include "Logger.h"
#include <cmath>

const LoggerPtr forcelog(log4cxx::Logger::getLogger("molsim.particle.force"));

LennardJones::LennardJones() {
	sigma11 = 1.0;
	epsilon11 = 5.0;

}

LennardJones::LennardJones(Simulation *s) {
	setSimulation(s);
}

LennardJones::~LennardJones() {

}

void LennardJones::calc(Particle& p1, Particle& p2) {
	double sigma, epsilon;
	if ((p1.getType() == 0) && (p2.getType() == 0)) {
		sigma = sigma11;
		epsilon = epsilon11;
	}
	else if ((p1.getType() == 1) && (p2.getType() == 1)) {
		sigma = sigma22;
		epsilon = epsilon22;
	}
	else if (p1.getType() != p2.getType()) {
		sigma = sigma12;
		epsilon = epsilon12;
	}
	double coeff = 24 * epsilon;

	utils::Vector<double, 3>& f1 = p1.getF();
	utils::Vector<double, 3>& f2 = p2.getF();

	utils::Vector<double, 3> diff = p2.getX() - p1.getX();
	double norm_inverse = 1 / diff.L2Norm();
	norm_inverse *= norm_inverse;	// square
	double field_pow = pow(sigma, 6) * pow(norm_inverse, 3);	// (sigma / norm)^6
	utils::Vector<double, 3> f = coeff * norm_inverse * field_pow * (1 - 2*field_pow) * diff;

	f1 = f1 + f;
	f2 = f2 - f;
}

void LennardJones::setSimulation(Simulation *s) {
	sim = s;
	sigma11 = sim->sigmas[0];
	sigma22 = sim->sigmas[1];
	sigma12 = (sigma11 + sigma22) / 2;

	epsilon11 = sim->epsilons[0];
	epsilon22 = sim->epsilons[1];
	epsilon12 = sqrt(epsilon11 * epsilon22);
}


LennardJonesLC::LennardJonesLC() {
	LennardJones::LennardJones();
}

LennardJonesLC::LennardJonesLC(Simulation *s) {
	setSimulation(s);
}

LennardJonesLC::~LennardJonesLC() {

}

void LennardJonesLC::calc(Particle& p1, Particle& p2) {
	double sigma, epsilon;
	if ((p1.getType() == 0) && (p2.getType() == 0)) {
		sigma = sigma11;
		epsilon = epsilon11;
	}
	else if ((p1.getType() == 1) && (p2.getType() == 1)) {
		sigma = sigma22;
		epsilon = epsilon22;
	}
	else if (p1.getType() != p2.getType()) {
		sigma = sigma12;
		epsilon = epsilon12;
	}
	double coeff = 24 * epsilon;

	//LOG4CXX_DEBUG(forcelog, "force calc LC");
	utils::Vector<double, 3>& f1 = p1.getF();

	utils::Vector<double, 3> diff = p2.getX() - p1.getX();
	double norm_inverse = 1 / diff.L2Norm();
	norm_inverse *= norm_inverse;	// square
	double field_pow = pow(sigma, 6) * pow(norm_inverse, 3);	// (sigma / norm)^6
	utils::Vector<double, 3> f = coeff * norm_inverse * field_pow * (1 - 2*field_pow) * diff;

	f1 = f1 + f;
}
