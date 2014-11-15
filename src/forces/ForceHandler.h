/*
 * ForceHandler.h
 *
 *  Created on: 22.10.2014
 *      Author: christiansturm
 */

#ifndef SRC_FORCECALC_H_
#define SRC_FORCECALC_H_

#include "../Particle.h"

/* based on strategy pattern */
/** An abstract class every actual force calculator (e.g. Gravitation) has to implement */
class ForceHandler {
public:
	/** destructor */
	virtual ~ForceHandler();

	/** calls previously set force calculation
	 * @param p1, p2 Particles to calculate the force*/
	virtual void calc(Particle& p1, Particle& p2) = 0;
};

#endif /* SRC_FORCECALC_H_ */
