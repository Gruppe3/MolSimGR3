/*
 * ParticleContainerLC.h
 *
 *  Created on: 16.11.2014
 *      Author: ninaavr
 */
//NOTE:compiles if particles,iterator and othersIterator from ParticleContainer are public
#ifndef SRC_PARTICLECONTAINERLC_H_
#define SRC_PARTICLECONTAINERLC_H_

#include "ParticleContainer.h"
#include <list>
using namespace std;
/**defines dimenstion of the grid*/
#define DIM 3
/**stores dynamic particle list for each cell*/
typedef struct ParticleList {
	Particle *p;
	struct ParticleList *next;
} ParticleList;

/** type of cell with pointer to first element (if existing) */
typedef struct Cell {
	ParticleList *root;
} Cell;

/** types of boundary conditions */
enum Boundary {OUTFLOW, REFLECTING};
/** stores sides of the 3d domain;
 * Attention: Don't change the order! */
enum domainSide {LEFT, RIGHT, BOTTOM, TOP, FRONT, BACK};

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

class ParticleContainerLC: public ParticleContainer {
private:
	/**points to current Particle in a given (current) cell*/
	ParticleList * particleIteratorInCell;
	/**points to Particle(List) last returned by next() */
	ParticleList * returnedParticleIteratorInCell;
	/**pointer to other Particle in a given (not necessary current)cell*/
	ParticleList * otherParticleIteratorInCell;
	/**pointer to current halo particle*/
	ParticleList * haloIteratorInCell;
	/**pointer to array of cells*/
	Cell *cells;
	/**pointer to array of halo cells*/
	Cell *haloCells;
	/** pointer to array of all cells (incl. halo) */
	Cell** allCells;
	/**cutoff radius of the force*/
	double radius;
	/** distance of particles (2^(1/6)*sigma) */
	double distance;
	/** size of the domain */
	double domainSize[DIM];
	/** boundary conditions for all sides of the 3D domain */
	Boundary domainBoundary[6];
	/**number of cells in each dimension*/
	int cellNums[DIM];
	/**number of all cells in each dimension inclusive halo particles*/
	int allCellNums[DIM]; //not implemented yet
	/**number of all halo cells*/
	int haloAllCellNums;
	/**index in array of the current halo cell*/
	int haloIndex;
	/**beginning of linked list of a given cell different from central cell*/
	Cell *otherCurrentCell;
	/**beginning of a linked list in a given cell*/
	Cell *centralCell;
	/**beginning of a linked list in a given halo cell*/
	ParticleList currentHaloCell;
	/**beginning of otherCurrentCell according to central cell, helps to calculate indices for otherCellIndex*/
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
	int neighborCells;
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
	void moveParticles_LC(Cell** cells, int *nc, double *l);
	/**inserts particles in a cell*/
	void insertList(ParticleList **list, ParticleList *i);
	/**deletes particles from a cell*/
	void deleteList(ParticleList **q);
	/** returns true if there are boundary particles left */
	bool hasNextBoundaryWall();
	/** returns particle from next not empty boundary index
	 * @param fixed Dimension of the wall which doesn't change
	 * @param fixedValue Dimension fixed is set to this;  common values: 0 or cellNums[fixed]-1 */
	Particle* nextBoundaryWall(int fixed, int fixedValue);
	/** 3D implementation of hasNextBoundary() */
	bool hasNextBoundary3D();
	/** 3D implementation of nextBoundary() */
	Particle* nextBoundary3D();
	/** 2D implementation of hasNextBoundary() */
	bool hasNextBoundary2D();
	/** 2D implementation of nextBoundary() */
	Particle* nextBoundary2D();
	void applyToBoundaryWall(int fixedDim, int fixedVal, PCApply *fnc);
	bool hasNextInHaloCell();
	Particle& nextInHaloCell();
	/** converts 1D index to DIM dimensional array index
	 * @param idx 1D index
	 * @param arr DIM dimensional array to store calculated coords.
	 * @param size Size of each dimension of arr */
	void numToIndex(int idx, int arr[], int size[]);

public:
	/** constructor */
	ParticleContainerLC();
	/** destructor */
	virtual ~ParticleContainerLC();
	/** creates ParticleContainerLC from cutoff radius, domainSize boundary conditions
	 *  and particles of another container */
	ParticleContainerLC(const double radius, double distance, double *domainSize,
			Boundary *boundary, const ParticleContainer* pc);
/** Calculates index of the cell, where the particle should be added and inserts it its linked list
	* @param p particle to add */
	virtual void add(Particle& p);
	/** returns number of particles in cells[] */
	virtual int size();
	/**Returns true if there are particles (different from current particle) left in all the cells*/
	virtual bool hasNextOther();
	/**Returns next particle (different from current particle) */
	virtual Particle& nextOther();
	/**Returns true if there are more particles left*/
	virtual bool hasNext();
	/**Returns next particle*/
	virtual Particle& next();
	/**Sets indices for inner particles to 0 and resets cell-selectCell()*/
	virtual void resetIterator();
	/**Sets indices for boundary particles to 0 and resets cell-selectCell()*/
	void resetBoundaryIterator();
	/**Returns true if there are more boundary particles left*/
	bool hasNextBoundary();
	/**Returns next boundary particle*/
	Particle* nextBoundary();
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

	/** removes all particles stored in halo cells */
	void emptyHalo();
	/** moves particles into right cell; call from outside */
	void moveParticles();

	/** applies fnc->iterateFunc on every particle in the domain
	 * @param fnc Pointer to a PCApply deriving object implementing the iterateFunc method */
	virtual void iterate(PCApply *fnc);
	/** applies fnc->iteratePairFunc on every particle pair according to the cutoff radius
	 * @param fnc Pointer to a PCApply deriving object implementing the iteratePairFunc method */
	virtual void iteratePair(PCApply *fnc);
	/** applies fnc->iterateFunc on boundary particles in the domain according to the given boundary type
	 * @param bd Only domain sides of this boundary type are affected
	 * @param fnc Pointer to a PCApply deriving object implementing the iterateFunc method */
	void applyBoundaryConds(Boundary bd, PCApply *fnc);
};

#endif /* SRC_PARTICLECONTAINERLC_H_ */
