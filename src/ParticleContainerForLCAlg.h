/*
 * ParticleContainerForLCAlg.h
 *
 *  Created on: 16.11.2014
 *      Author: ninaavr
 */
//NOTE:compiles if particles,iterator and othersIterator from ParticleContainer are public
#ifndef SRC_PARTICLECONTAINERFORLCALG_H_
#define SRC_PARTICLECONTAINERFORLCALG_H_

#include "ParticleContainer.h"
#include <list>
using namespace std;
/**defines dimenstion of the grid*/
#define DIM 3
/**stores dynamic particle list for each cell*/
typedef struct ParticleList {
	Particle p;
	struct ParticleList *next;
} ParticleList;

/**calculates indices of the array of cells
 * @param ic index of the cell in each dimension
 * @param nc number of cells in each dimension*/
#if 1==DIM
#define calcIndex(ic,nc) ((ic)[0])
#define numcell(nc) ((nc)[0])
#elif 2==DIM
#define calcIndex(ic,nc) ((ic)[0] + (nc)[0]*(ic)[1])
#define numcell(nc) ((nc)[0]*(nc)[1])
#elif 3==DIM
#define calcIndex(ic,nc) ((ic)[0] + (nc)[0]*((ic)[1] + (nc)[1]*(ic)[2]))
#define numcell(nc) ((nc)[0]*(nc)[1]*(nc)[2])
#endif

class ParticleContainerForLCAlg: public ParticleContainer {
private:
	/**points to current Particle in a given (current) cell*/
	ParticleList * particleIteratorInCell;
	/**pointer to other Particle in a given (not necessary current)cell*/
	ParticleList * otherParticleIteratorInCell;
	/**pointer to current halo particle*/
	ParticleList * haloIteratorInCell;
	/**pointer to array of cells*/
	ParticleList * cells;
	/**pointer to array of halo cells*/
	ParticleList * haloCells;
	/**cutoff radius of the force*/
	double radius;
	/**number of cells in each dimension*/
	int cellNums[];
	/**number of all cells in each dimension inclusive halo particles*/
	int allCellsNums[]; //not implemented yet
	/**number of all halo cells*/
	int halloAllCellNums;
	/**index in array of the current halo cell*/
	int haloIndex;
	/**beginning of linked list of a given cell different from central cell*/
	ParticleList otherCurrentCell;
	/**beginning of a linked list in a given cell*/
	ParticleList centralCell;
	/**beginning of a linked list in a given halo cell*/
	ParticleList currentHaloCell;
	/**beginning of otherCurrentCell according to central cell, helps to calcuate indices for otherCellIndex*/
	int beginningOtherCellIndex[DIM];
	/**index for other CurrentCell different from central cell*/
	int otherCellIndex[DIM];
	/**index for central cell*/
	int centralCellIndex[DIM];
	/**index in array of the current boundary cell*/
	int boundaryIndex[DIM];
	//help-variable in nextBoundary3D,index of current wall with boundary particles
	int boundaryCurrent;
	//help-variables in nextBoundaryWall, define in which dimension to iterate in the 3D array-boundaryIndex
	int xc, yc;
	//help-variable, helps calculating conditions in hasNextOther()
	int NeighborCells;
	/** Returns true if the iterator is not at the end of the linked list.*/
	bool hasNextInCell();
	/**Returns next particle in the linked list of the centralCell cell */
	Particle& nextInCell();
	/**Returns true if there are particles left in the linked list of the other particle.*/
	bool hasNextOtherInCell();
	/**Returns next particle in the linked list different from current particle.*/
	Particle& nextOtherInCell();
	/**resets cell
	 * @param cellNum index of the cell in array*/
	void selectCell(int cellNum[]);
	/**moves particle from one cell to another if necessary*/
	void moveParticles_LC(ParticleList cells[], int *nc, double *l);
	/**inserts particles in a cell*/
	void insertList(ParticleList *list, ParticleList *i);
	/**deletes particles from a cell*/
	void deleteList(ParticleList *q);
	bool hasNextBoundaryWall();
	Particle & nextBoundaryWall(int fixed, int fixedValue);
	bool hasNextBoundary3D();
	Particle & nextBoundary3D();
	bool hasNextBoundary2D();
	Particle & nextBoundary2D();
	bool hasNextInHaloCell();
	Particle& nextInHaloCell();

public:
	ParticleContainerForLCAlg();
	virtual ~ParticleContainerForLCAlg();
	ParticleContainerForLCAlg(double radius, double * domainSize,
			const ParticleContainer& pc);
/** Calcutaes index of the cell, where the particle should be added and inserts it its linked list
	* @param p particle to add */
	void add(Particle& p);
	/**Returns true if there are particles (different from current particle) left in all the cells*/
	bool hasNextOther();
	/**Returns next particle (different from current particle) */
	Particle& nextOther();
	/**Returns true if there are more particles left*/
	bool hasNext();
	/**Returns next particle*/
	Particle& next();
	/**Sets indices for inner particles to 0 and resets cell-selectCell()*/
	void resetIterator();
	/**Sets indices for boundary particles to 0 and resets cell-selectCell()*/
	void resetBoundaryIterator();
	/**Returns true if there are more boundary particles left*/
	bool hasNextBoundary();
	/**Returns next boundary particle*/
	Particle& nextBoundary();
	/**adds particle in separate array of cells with lists of particles
	 * @ p particle to add
	 * @ i index, that defines in which cell to add the particle*/
	void addHalo(Particle& p, int i);
	/**Sets indices for halo particles to 0*/
	void resetHaloIterator();
	/**Returns true if there are more halo particles left.*/
	bool hasNextHaloParticle();
	/**returns next halo particle*/
	Particle& nextHaloParticle();
	/**iterator to remove all particles one after the other*/
	void removeNextHalo();
	/**removes specific particle
	 * @ p particle to remove*/
	void removeHaloParticle(Particle& p);

};

#endif /* SRC_PARTICLECONTAINERFORLCALG_H_ */
