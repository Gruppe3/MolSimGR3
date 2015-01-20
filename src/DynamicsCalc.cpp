/*
 * DynamicsCalc.cpp
 *
 *  Created on: 26.11.2014
 *      Author: christiansturm
 */

#include "DynamicsCalc.h"

CalcX::~CalcX() {

}

CalcV::~CalcV() {

}

CalcT::~CalcT(){

}

void CalcX::iterateFunc(Particle& p) {
	int type = p.getType();
	if (sim->states[type]!="solid"){
		utils::Vector<double, 3>& x = p.getX();
		x = x + sim->delta_t * (p.getV() + sim->delta_t / (2 * p.getM()) * p.getF());
	}
}

void CalcV::iterateFunc(Particle& p) {
	utils::Vector<double, 3>& v = p.getV();
	v = (v + sim->delta_t / (2 * p.getM()) * (p.getOldF() + p.getF()))*beta ;
	//looks like velocity scaling has to be applied for all particles inclusive solid
	//otherwise wall particles get too big velocities
}

void CalcV::setBeta(double beta_tag) {
	beta = beta_tag;
}


void CalcT::iterateFunc(Particle& p) {
	double vnorm = p.getV().L2NormSquared();
	energy = energy + p.getM() * vnorm ;
	if (sim->states[p.getType()]=="liquid"){
		sim->flowKineticEnergy += p.getV()[1]*p.getV()[1]*p.getM();
		sim->numParticles++;
	}
}



double CalcT::calcBeta(double tD, int numParticles){
	double T = (energy - sim->flowKineticEnergy)/ (DIM * numParticles * sim->boltzmannConst);
	//cout<<"Temperature:   "<<T<<endl;
	return sqrt(tD / T);
}

void CalcT::resetEnergy(){
	energy = 0;
	sim->flowKineticEnergy = 0;
	sim->numParticles = 0;
}

void CalcT::initializeV(double *v, double tInit, double m) {
	*v = sqrt(tInit / m);
}
