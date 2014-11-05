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

class LennardJones : public ForceHandler {
public:
	/** constructor */
	LennardJones();
	/** destructor */
	virtual ~LennardJones();

	/** calculates the LJ force between the particles and adds it to their total forces
	 * @param p1,p2 Particles for LJ force */
	void calc(Particle& p1, Particle& p2);
private:
	double sigma;
	double epsilon;
	double coeff;
};

#endif /* SRC_FORCES_LENNARDJONES_H_ */
