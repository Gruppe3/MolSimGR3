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
#include "Simulation.h"

using namespace std;

/** Abstract class for calculation on particles of the ParticleContainer;
 * Functions have to be handed over to the iterate/iteratePair methods of
 * the ParticleContainer.*/
class PCApply {
public:
	PCApply() {
		sim = NULL;
	}

	/** constructor */
	PCApply(Simulation *s) {
		sim = s;
	}

	/** destructor */
	virtual ~PCApply() {}

	/** function which calculates something on the particle p */
	virtual void iterateFunc(Particle& p) {};
	/** function that calculates something on the pair p1, p2 */
	virtual void iteratePairFunc(Particle& p1, Particle& p2) {};
protected:
	Simulation *sim;
};

/* based on iterator pattern*/
/** Stores an amount of particles and provides functions for iteration */
class ParticleContainer {
public:
//private:
	/** list of particles */
	list<Particle> particles;

	/** iterator to determine next (current) particle */
	list<Particle>::iterator iterator;

	/** iterator pointing to next particle different from current particle */
	list<Particle>::iterator othersIterator;

//public:
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
	virtual void add(Particle& p);

	/** Returns number of particles in the list */
	virtual int size();

	/** Returns true if the iterator is not at the end of the list.
	 * Has to be called before next() to avoid pointer errors */
	virtual bool hasNext();

	/** Returns next particle in the list (iterator). Also updates otherIterator to the following particle */
	virtual Particle& next();

	/** Returns true if there are particles left in the list different from the current particle.
	 * Has to be called before nextOther() to avoid pointer errors*/
	virtual bool hasNextOther();

	/** Returns next particle different from current particle (otherIterator) */
	virtual Particle& nextOther();

	/** Sets ALL internal iterators to the beginning of the list */
	virtual void resetIterator();
	/** Applies function on all stored particles */
	virtual void iterate(PCApply *fnc);
	/** Applies function on all particle pairs */
	virtual void iteratePair(PCApply *fnc);
};

#endif /* SRC_PARTICLECONTAINER_H_ */
