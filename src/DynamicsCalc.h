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
public:
	/** constructor */
	CalcV(Simulation *s) {
		sim = s;
	}

	virtual ~CalcV();
	void iterateFunc(Particle& p);
};

#endif /* SRC_DYNAMICSCALC_H_ */
