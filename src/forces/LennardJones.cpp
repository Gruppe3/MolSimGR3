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
LennardJonesLC::LennardJonesLC(Simulation *sim) {
sigma11=sim->sigma11;
sigma22=sim->sigma22;
epsilon11=sim->epsilon11;
epsilon22=sim->epsilon22;
sigma12=(sigma11+sigma22)/2;
epsilon12=sqrt(epsilon11*epsilon22);
}



LennardJonesLC::~LennardJonesLC() {

}

void LennardJonesLC::calc(Particle&p1, Particle& p2) {
		if((p1.getType()==0)&&(p2.getType()==0)){
		sigma=sigma11;
		epsilon=epsilon11;
		coeff=24*epsilon11;
		//LOG4CXX_DEBUG(forcelog, "sigma11 is: "<<sigma<<"  epsilon11 is:  "<<epsilon);
	}else if((p1.getType()==1)&&(p2.getType()==1)){
		sigma=sigma22;
		epsilon=epsilon22;
		coeff=24*epsilon22;
		//LOG4CXX_DEBUG(forcelog, "sigma22 is: "<<sigma<<"  epsilon22 is:  "<<epsilon);
	}else if(p1.getType()!=p2.getType()){
		sigma=sigma12;
		epsilon=epsilon12;
		coeff=24*epsilon12;
		//LOG4CXX_DEBUG(forcelog, "sigma12 is: "<<sigma<<"  epsilon12 is:  "<<epsilon);
	}
	//LOG4CXX_DEBUG(forcelog, "force calc LC");
	utils::Vector<double, 3>& f1 = p1.getF();
	utils::Vector<double, 3>& f2 = p2.getF();

	utils::Vector<double, 3> diff = p2.getX() - p1.getX();
	double norm_inverse = 1 / diff.L2Norm();
	norm_inverse *= norm_inverse;	// square
	double field_pow = pow(sigma, 6) * pow(norm_inverse, 3);	// (sigma / norm)^6
	utils::Vector<double, 3> f = coeff * norm_inverse * field_pow * (1 - 2*field_pow) * diff;
	//LOG4CXX_DEBUG(forcelog, "coeff(24*epsilon) is: "<<coeff);
	f1 = f1 + f;
}
