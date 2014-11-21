/*
 * LennardJones.cpp
 *
 *  Created on: 02.11.2014
 *      Author: christiansturm
 */

#include "LennardJones.h"
#include "Logger.h"
#include <cmath>

extern const LoggerPtr forcelog;

LennardJones::LennardJones() {
	sigma = 1.0;
	epsilon = 5.0;
	coeff = 24 * epsilon;
}

LennardJones::~LennardJones() {

}

void LennardJones::calc(Particle&p1, Particle& p2) {
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



LennardJonesLC::LennardJonesLC() {
	sigma = 1.0;
	epsilon = 5.0;
	coeff = 24 * epsilon;
}

LennardJonesLC::~LennardJonesLC() {

}

void LennardJonesLC::calc(Particle&p1, Particle& p2) {
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
