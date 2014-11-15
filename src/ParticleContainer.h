/*
 * ParticleContainer.h
 *
 *  Created on: 20.10.2014
 *      Author: christiansturm
 */

#ifndef SRC_PARTICLECONTAINER_H_
#define SRC_PARTICLECONTAINER_H_

#include "Particle.h"
#include <list>
#include <log4cxx/logger.h>

using namespace std;
using namespace log4cxx;

/* based on iterator pattern*/
/** Stores an amount of particles and provides functions for iteration */
class ParticleContainer {
private:
	/** list of particles */
	list<Particle> particles;

	/** iterator to determine next (current) particle */
	list<Particle>::iterator iterator;

	/** iterator pointing to next particle different from current particle */
	list<Particle>::iterator othersIterator;

public:
	/** constructs new empty container */
	ParticleContainer();

	/** constructs container with given particle list
	 * @param l the particle list that should be used by the container */
	ParticleContainer(list<Particle> l);

	/** constructs a copy of an existing container
	 * @param pc the particle container that should be copied */
	ParticleContainer(const ParticleContainer& pc);

	/** destructor */
	virtual ~ParticleContainer();

	/** Adds particle p to the end of the list
	 * @param p The new particle */
	void add(Particle& p);

	/** Returns number of particles in the list */
	int size();

	/** Returns true if the iterator is not at the end of the list.
	 * Has to be called before next() to avoid pointer errors */
	bool hasNext();

	/** Returns next particle in the list (iterator). Also updates otherIterator to the following particle */
	Particle& next();

	/** Returns true if there are particles left in the list different from the current particle.
	 * Has to be called before nextOther() to avoid pointer errors*/
	bool hasNextOther();

	/** Returns next particle different from current particle (otherIterator) */
	Particle& nextOther();

	/** Sets ALL internal iterators to the beginning of the list */
	void resetIterator();
};

#endif /* SRC_PARTICLECONTAINER_H_ */
