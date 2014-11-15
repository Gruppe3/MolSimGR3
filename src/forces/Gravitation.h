/*
 * Gravitation.h
 *
 *  Created on: 22.10.2014
 *      Author: christiansturm
 */

#ifndef SRC_FORCES_H_
#define SRC_FORCES_H_

#include "ForceHandler.h"
#include "../Particle.h"
#include "../utils/Vector.h"

/** A force calculator using the gravitational potential */
class Gravitation : public ForceHandler {
public:
	/** constructor */
	Gravitation();
	/** destructor */
	virtual ~Gravitation();

	/** calculates the gravitational force between two particles and adds it to their total forces
	 * @param p1,p2 Particles to calculate the gravitation */
	void calc(Particle& p1, Particle& p2);
};

#endif /* SRC_FORCES_H_ */
