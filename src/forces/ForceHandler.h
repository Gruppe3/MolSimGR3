/*
 * ForceHandler.h
 *
 *  Created on: 22.10.2014
 *      Author: christiansturm
 */

#ifndef SRC_FORCECALC_H_
#define SRC_FORCECALC_H_

#include "../Particle.h"
#include "../ParticleContainer.h"

/* based on strategy pattern */
/** An abstract class every actual force calculator (e.g. Gravitation) has to implement */
class ForceHandler : public PCApply {
public:
	/** destructor */
	virtual ~ForceHandler();

	/** calls previously set force calculation
	 * @param p1, p2 Particles to calculate the force*/
	virtual void calc(Particle& p1, Particle& p2) = 0;

	/** calculates force for particles using iteration methods of ParticleContainer */
	virtual void iteratePairFunc(Particle& p1, Particle& p2) {
		calc(p1, p2);
	}
	/** resets forces for all particles using iteration methods of ParticleContainer */
	void iterateFunc(Particle& p) {
		utils::Vector<double, 3>& f = p.getF();
		utils::Vector<double, 3>& f_old = p.getOldF();
		f_old = f;
		f = 0.0;
	}

	virtual void setSimulation(Simulation *s) {
		sim = s;
	}

protected:
	Simulation *sim;
};

#endif /* SRC_FORCECALC_H_ */
