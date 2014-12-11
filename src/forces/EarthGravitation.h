/*
 * EarthGravitation.h
 *
 *  Created on: 07.12.2014
 *      Author: vmuser
 */

#ifndef SRC_FORCES_EARTHGRAVITATION_H_
#define SRC_FORCES_EARTHGRAVITATION_H_

#include <forces/ForceHandler.h>

class EarthGravitation: public ForceHandler {

private:
	double earthGravity;
public:
	EarthGravitation(Simulation* sim);
	virtual ~EarthGravitation();

	/** adds gravity to y dimension of p's force */
	void iterateFunc(Particle& p);
	virtual void calc(Particle& p1, Particle& p2);
};

#endif /* SRC_FORCES_EARTHGRAVITATION_H_ */
