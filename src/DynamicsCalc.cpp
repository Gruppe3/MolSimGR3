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
	utils::Vector<double, 3>& x = p.getX();
	x = x + sim->delta_t * (p.getV() + sim->delta_t / (2 * p.getM()) * p.getF());
}

void CalcV::iterateFunc(Particle& p) {
	utils::Vector<double, 3>& v = p.getV();
	v = (v + sim->delta_t / (2 * p.getM()) * (p.getOldF() + p.getF()))*beta;
}

void CalcV::setBeta(double beta_tag) {
	beta=beta_tag;
}

void CalcT::iterateFunc(Particle& p){
	double tmp=p.getV().L2Norm();
	sum=sum+p.getM()*tmp*tmp;
}

double CalcT::calcBeta(double tD,int numParticles){
	double T=sum/(DIM*numParticles*sim->boltzmannConst);
	//cout<<"Temperature:   "<<T<<endl;
	double beta = sqrt(tD/T);
	return beta;
}

void CalcT::resetSum(){
	sum=0;
}

void initializeV(double * v, double tInit, double m){
	*v=sqrt(tInit/m);
}
