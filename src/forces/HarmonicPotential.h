/*
 * harmonicPotential.h
 *
 *  Created on: 27.12.2014
 *      Author: vmuser
 */

#ifndef SRC_FORCES_HARMONICPOTENTIAL_H_
#define SRC_FORCES_HARMONICPOTENTIAL_H_

#include <forces/ForceHandler.h>
#include "../Particle.h"
#include "LennardJones.h"

class HarmonicPotential: public ForceHandler {
/*private:
	Simulation *s;*/
public:
	HarmonicPotential(Simulation* s);
	virtual ~HarmonicPotential();
	virtual void calc(Particle& p1, Particle& p2);
};

#endif /* SRC_FORCES_HARMONICPOTENTIAL_H_ */
