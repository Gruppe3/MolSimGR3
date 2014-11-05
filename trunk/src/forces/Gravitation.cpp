/*
 * Forces.cpp
 *
 *  Created on: 22.10.2014
 *      Author: christiansturm2
 */

#include "Gravitation.h"

Gravitation::Gravitation() {

}

Gravitation::~Gravitation() {

}

void Gravitation::calc(Particle& p1, Particle& p2) {
	utils::Vector<double, 3>& f1 = p1.getF();
	utils::Vector<double, 3>& f2 = p2.getF();

	utils::Vector<double, 3> diff = p2.getX() - p1.getX();
	double diff_norm = diff.L2Norm();
	utils::Vector<double, 3> f = p1.getM() * p2.getM() / (diff_norm * diff_norm * diff_norm) * diff;

	f1 = f1 + f;
	f2 = f2 - f;
}
