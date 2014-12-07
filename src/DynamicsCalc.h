/*
 * DynamicsCalc.h
 *
 *  Created on: 26.11.2014
 *      Author: christiansturm
 */

#ifndef SRC_DYNAMICSCALC_H_
#define SRC_DYNAMICSCALC_H_

#include "ParticleContainer.h"
#include "Simulation.h"
#include "ParticleContainerLC.h"

//#define boltzmannConst 1.3806488E-23
/** calculates new position of a particle using iteration method of ParticleContainer */
class CalcX : public PCApply {
public:
	/** constructor */
	CalcX(Simulation *s) {
		sim = s;
	}

	virtual ~CalcX();
	void iterateFunc(Particle& p);
};

/** calculates new velocity of a particle using iteration method of ParticleContainer */
class CalcV : public PCApply {
private:
	double beta;
public:
	/** constructor */
	CalcV(Simulation *s) {
		sim = s;
		beta=1;
	}

	virtual ~CalcV();
	void setBeta(double beta_tag) ;
	void iterateFunc(Particle& p);
};

class CalcT : public PCApply {
private:
	double sum;
public:
	/** constructor */
	CalcT(Simulation *s) {
		sim = s;
		sum=0;
	}
	virtual ~CalcT();
	void iterateFunc(Particle& p);
	void resetSum();
	double calcBeta(double tD, int numParticles);
	double calcBetaForDiffTarget(double tD, int numParticles, double tempDiff);
};
void initializeV(double * v, double tInit, double m);
#endif /* SRC_DYNAMICSCALC_H_ */
