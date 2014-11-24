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

/** A force calculator using the Lennard-Jones potential */
class LennardJones : public ForceHandler {
public:
	/** constructor */
	LennardJones();
	/** destructor */
	virtual ~LennardJones();

	/** calculates the LJ force between the particles and adds it to their total forces
	 * @param p1,p2 Particles for LJ force */
	virtual void calc(Particle& p1, Particle& p2);
private:
	double sigma;
	double epsilon;
	double coeff;
};

/** A force calculator using the Lennard-Jones potential for the LC algorithm (no Newton III) */
class LennardJonesLC : public ForceHandler {
public:
	/** constructor */
	LennardJonesLC();
	/** destructor */
	virtual ~LennardJonesLC();

	/** calculates the LJ force between the particles and adds it to their total forces
	 * @param p1,p2 Particles for LJ force */
	virtual void calc(Particle& p1, Particle& p2);
private:
	double sigma;
	double epsilon;
	double coeff;
};

#endif /* SRC_FORCES_LENNARDJONES_H_ */
