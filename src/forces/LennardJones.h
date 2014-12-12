/*
 * LennardJones.h
 *
 *  Created on: 02.11.2014
 *      Author: christiansturm
 */

#ifndef SRC_FORCES_LENNARDJONES_H_
#define SRC_FORCES_LENNARDJONES_H_

#include "../Particle.h"
#include "ForceHandler.h"
//#include "../Simulation.h"

/** A force calculator using the Lennard-Jones potential */
class LennardJones : public ForceHandler {
public:
	/** constructor */
	LennardJones();

	/** constructor */
	LennardJones(Simulation *s);

	/** destructor */
	virtual ~LennardJones();

	/** calculates the LJ force between the particles and adds it to their total forces
	 * @param p1,p2 Particles for LJ force */
	virtual void calc(Particle& p1, Particle& p2);

	/** get sigma and epsilon from s */
	virtual void setSimulation(Simulation *s);

protected:
	double sigma11;
	double sigma22;
	double epsilon11;
	double epsilon22;
	double sigma12;
	double epsilon12;
};

/** A force calculator using the Lennard-Jones potential for the LC algorithm (no Newton III) */
class LennardJonesLC : public LennardJones {
public:
	/** constructor */
	LennardJonesLC();
	/** constructor */
	LennardJonesLC(Simulation *sim);


	/** destructor */
	virtual ~LennardJonesLC();

	/** calculates the LJ force between the particles and adds it to their total forces
	 * @param p1,p2 Particles for LJ force */
	virtual void calc(Particle& p1, Particle& p2);

	virtual void iteratePairFunc(Particle& p1, Particle& p2) {
		calc(p1, p2);
	}
};

#endif /* SRC_FORCES_LENNARDJONES_H_ */
