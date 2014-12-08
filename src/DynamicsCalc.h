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
	/** scaling factor for velocity */
	double beta;
public:
	/** constructor */
	CalcV(Simulation *s) {
		sim = s;
		beta = 1;
	}

	virtual ~CalcV();

	/** sets beta */
	void setBeta(double beta_tag) ;

	/** calculates new velocity of particle p */
	void iterateFunc(Particle& p);
};

class CalcT : public PCApply {
private:
	/** total kinetic energy / 2 */
	double energy;
public:
	/** constructor */
	CalcT(Simulation *s) {
		sim = s;
		energy = 0;
	}
	virtual ~CalcT();

	/** calculates total kinetic energy */
	void iterateFunc(Particle& p);

	/** resets energy to 0 */
	void resetEnergy();

	/** calculates scaling factor for velocities */
	double calcBeta(double tD, int numParticles);

	/**  */
	double calcBetaForDiffTarget(double tD, int numParticles, double tempDiff);

	/** sets scaling value for Maxwell-Boltzmann distribution
	 * @param v pointer to velocity of particle
	 * @param tInit initial temperature
	 * @param m mass of particle*/
	static void initializeV(double *v, double tInit, double m);
};

#endif /* SRC_DYNAMICSCALC_H_ */
