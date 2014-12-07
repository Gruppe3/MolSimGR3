/*
 * EarthGravitation.cpp
 *
 *  Created on: 07.12.2014
 *      Author: vmuser
 */

#include <forces/EarthGravitation.h>

EarthGravitation::EarthGravitation() {
	earthGravity=9.807;
}

EarthGravitation::~EarthGravitation() {

}

void EarthGravitation::iterateFunc(Particle& p) {
		utils::Vector<double, 3>& f = p.getF();
		f[1]=f[1]+earthGravity*p.getM();
	}
void EarthGravitation::calc(Particle& p1, Particle& p2){

}
