/*
 * ParticleContainerForLCAlg.cpp
 *
 *  Created on: 16.11.2014
 *      Author: vmuser
 */

#include "ParticleContainerLC.h"
#include "Logger.h"

extern const LoggerPtr particlelog;

ParticleContainerLC::ParticleContainerLC() {
	// TODO Auto-generated constructor stub

}

ParticleContainerLC::~ParticleContainerLC() {
	delete [] cells;
	delete [] haloCells;
	delete [] allCells;//free(allCells);
}

ParticleContainerLC::ParticleContainerLC(double rad,
		double *domain, const ParticleContainer* pc) {
	//NOTE:compiles if particles,iterator and othersIterator from ParticleContainer are public
	particles = pc->particles;
	iterator = pc->iterator;
	othersIterator = pc->othersIterator;

	for (int i = 0; i < DIM; i++) {
		domainSize[i] = domain[i];
	}
	radius = rad;

	for (int d = 0; d < DIM; d++) {
		int expansion = domainSize[d] / radius;
		cellNums[d] = expansion > 0 ? expansion : 1;	// expansion in dimension d must be at least 1 for correct numcell()
		allCellNums[d] = cellNums[d] + 2;
	}
#if 1==DIM
	allCellNums[0] = cellNums[0] + 2;
#elif 2==DIM
	haloAllCellNums= /*2*((cellNums[0]+2)+(cellNums[1])+2)*/ (cellNums[0] + 2) * (cellNums[1] + 2) - cellNums[0]*cellNums[1];
#elif 3==DIM
	haloAllCellNums = /*2
			* (((cellNums[0] + 2) * (cellNums[1] + 2))
					+ (cellNums[1] * cellNums[2] + 2)
					+ (cellNums[0] * cellNums[2] + 2));*/ (cellNums[0] + 2) * (cellNums[1] + 2) * (cellNums[2] + 2) - cellNums[0]*cellNums[1]*cellNums[2];
#endif
	cells = new Cell[numcell(cellNums)];
	haloCells = new Cell[haloAllCellNums];
	allCells = new Cell*[numcell(allCellNums)];//(Cell**) malloc(sizeof(Cell*) * numcell(allCellNums));

	// connect allCells to cells and haloCells
	int i = 0;
	int ic[DIM], kc[DIM];
	for (ic[0] = 0; ic[0] < allCellNums[0]; ic[0]++)
#if 1<DIM
		for (ic[1] = 0; ic[1] < allCellNums[1]; ic[1]++)
#endif
#if 3==DIM
			for (ic[2] = 0; ic[2] < allCellNums[2]; ic[2]++)
#endif
			{
				bool isHalo = false;
				for (int d = 0; d < DIM; d++) {
					isHalo = isHalo || ic[d] == 0 || ic[d] == allCellNums[d]-1;
				}
				if (isHalo) {
					haloCells[i].root = NULL;
					allCells[calcIndex(ic, allCellNums)] = &haloCells[i];
					i++;
					//LOG4CXX_DEBUG(particlelog, "halo " << i << "of " << haloAllCellNums << " all:" << numcell(allCellNums) << " inner:" << numcell(cellNums));
				}
				else {
#if 1==DIM
					int tmp[DIM] = {ic[0]-1};
#elif 2==DIM
					int tmp[DIM] = {ic[0]-1, ic[1]-1};

#elif 3==DIM
					int tmp[DIM] = {ic[0]-1, ic[1]-1, ic[2]-1};
#endif
					cells[calcIndex(tmp, cellNums)].root = NULL;
					allCells[calcIndex(ic, allCellNums)] = &cells[calcIndex(tmp, cellNums)];
				}
			}
	int count = 0;
	ParticleContainer::resetIterator();
	while (ParticleContainer::hasNext()) {
		count++;
		Particle& p = ParticleContainer::next();
#if 1==DIM
		int particleIndex[]= {p.getX()[0] / radius};
#elif 2==DIM
		int particleIndex[]= {p.getX()[0] / radius, p.getX()[1] / radius};
#elif 3==DIM
		int particleIndex[] = { p.getX()[0] / radius, p.getX()[1] / radius,
				p.getX()[2] / radius };
#endif
		ParticleList* pl = new ParticleList;
		pl->p = &p;
		pl->next = NULL;
		insertList((ParticleList**)&(cells[calcIndex(particleIndex, cellNums)].root), pl);
		//LOG4CXX_DEBUG(particlelog, "add p: " << p.getX().toString() << " to index " << particleIndex[0] << "," << particleIndex[1] << "," << particleIndex[2]);
		//LOG4CXX_DEBUG(particlelog, "root: " << cells[calcIndex(particleIndex, cellNums)].root->p->getX().toString());
	}
	resetIterator();
	//resetBoundaryIterator();
	//resetHaloIterator();

	LOG4CXX_DEBUG(particlelog, "grid: " << cellNums[0] << " x " << cellNums[1] << " x " << cellNums[2] << ", inserted count:" << count << ", size() delivers:" << size());
}

int ParticleContainerLC::size() {
	int count = 0;
	for (int i = 0; i < numcell(cellNums); i++) {
		ParticleList *q = cells[i].root;
		while (q != NULL) {
			count++;
			q = q->next;
		}
	}
	return count;
}

//PROBLEM:we don't know where the seperate grids begin: p.getX()[0]/radius is not correct there is also shift with 20 or 70 for our example
void ParticleContainerLC::add(Particle& p) {
	particles.push_back(p);
	resetIterator();

	// define index of cell for particle
#if 1==DIM
	int particleIndex[]= {p.getX()[0]/radius};
#elif 2==DIM
	int particleIndex[]= {p.getX()[0]/radius,p.getX()[1]/radius};
#elif 3==DIM
	int particleIndex[] = { p.getX()[0] / radius, p.getX()[1] / radius,
			p.getX()[2] / radius };
#endif
	ParticleList* pl = new ParticleList;;
	pl->p = &p;
	pl->next = NULL;
	insertList((ParticleList**)&(cells[calcIndex(particleIndex, cellNums)].root), pl);
}

void ParticleContainerLC::selectCell(int celli[]) {
	for (int i = 0; i < DIM; i++) {
		beginningOtherCellIndex[i] = celli[i]; // incl. additional +1 for allCells instead of cells (beginningOtherCellIndex in allCells coords.)
		if (beginningOtherCellIndex[i] < 0) {
			beginningOtherCellIndex[i] = 0;
		}
		otherCellIndex[i] = beginningOtherCellIndex[i];
	}
	otherCurrentCell = allCells[calcIndex(beginningOtherCellIndex, allCellNums)];
	otherParticleIteratorInCell = otherCurrentCell->root;
	//LOG4CXX_DEBUG(particlelog, "select cell before, root:" << cells[calcIndex(celli, cellNums)].root);
	centralCell = &cells[calcIndex(celli, cellNums)];
	particleIteratorInCell = centralCell->root;
	returnedParticleIteratorInCell = particleIteratorInCell;

	//LOG4CXX_DEBUG(particlelog, "select cell after:" << celli[0]+1 << "," << celli[1]+1 << "," << celli[2]+1);
}

void ParticleContainerLC::insertList(ParticleList **list,
		ParticleList *i) {
	i->next = *list;
	*list = i;
}

void ParticleContainerLC::deleteList(ParticleList **q) {
	*q = (*q)->next;
}

void ParticleContainerLC::moveParticles_LC(Cell** cell_arr, int *nc,
		double *l) {
	int dim = DIM;
	int ic[dim], kc[dim];
	for (ic[0] = 0; ic[0] < nc[0]; ic[0]++)
		for (ic[1] = 0; ic[1] < nc[1]; ic[1]++)
#if 3==DIM
			for (ic[2] = 0; ic[2] < nc[2]; ic[2]++)
#endif
					{
				//LOG4CXX_DEBUG(particlelog, "move() index: " << calcIndex(ic, nc) << " of " << numcell(allCellNums));
				ParticleList **q = &cell_arr[calcIndex(ic, nc)]->root;	// get cell/ first ParticleList at index ic
				ParticleList *i = *q;
				//LOG4CXX_DEBUG(particlelog, "got i in move():" << q);
				while (NULL != i) {
					Particle& p = *i->p;
					//LOG4CXX_DEBUG(particlelog, "l:" << l[0] << "," << l[1] << "," << l[2]);
					// kc: 3d index of cell which has to store i->p according to the X vector of p
					//LOG4CXX_DEBUG(particlelog, "next p in move():" << i->p->getX().toString());
					utils::Vector<double, 3> X = p.getX();
					for (int d = 0; d < DIM; d++) {
						int coord = ((int) floor(X[d] / radius)) + 1;	// +1 for transformation from cells[] to allCells[] coords.
						kc[d] = coord < nc[d] ? (coord < 0 ? 0.0 : coord) : nc[d]-1;	// indices must fit into cell_arr[]
					}
					if ((ic[0] != kc[0]) || (ic[1] != kc[1])
#if 3==DIM
							|| (ic[2] != kc[2])
#endif
							) {
						//LOG4CXX_DEBUG(particlelog, "moving from "<<ic[0]<<","<<ic[1]<<","<<ic[2] << " to " <<kc[0]<<","<<kc[1]<<","<<kc[2]);
						// move particle into cell kc
						deleteList(q);

						//LOG4CXX_DEBUG(particlelog, "deleted from old cell; old index:" << calcIndex(ic, nc) << "; new index:" << calcIndex(kc, nc));
						insertList(&cell_arr[calcIndex(kc, nc)]->root, i);
						//LOG4CXX_DEBUG(particlelog, "inserted in new cell");
					} else {//LOG4CXX_DEBUG(particlelog, "next p in move()");
						q = &i->next;
					}
					i = *q;
				}
			}
}

void ParticleContainerLC::moveParticles() {
	moveParticles_LC(allCells, allCellNums, domainSize);
}

bool ParticleContainerLC::hasNext() {
	// test for next particle in same cell
	if (hasNextInCell()) {
		//LOG4CXX_DEBUG(particlelog, "next in cell:" << particleIteratorInCell->p->getX().toString() << ", current:"  << returnedParticleIteratorInCell->p->getX().toString() << ", cell:" << centralCellIndex[0] << "," << centralCellIndex[1] << "," << centralCellIndex[2]);
		return true;
	}
	//LOG4CXX_DEBUG(particlelog, "has no next in cell " << centralCellIndex[0] << "," << centralCellIndex[1] << "," << centralCellIndex[2]);
	// test following cells for particles
	int idx = calcIndex(centralCellIndex, cellNums)+1;	// index of next cell in cells[]
	while (idx < numcell(cellNums)) {
		if (cells[idx].root != NULL)
			return true;
		idx++;
	}

	return false;
}

Particle& ParticleContainerLC::next() { //nextInDomain
	while (!hasNextInCell()) {	// switch to next cell if no particles left in centralCell
		//LOG4CXX_DEBUG(particlelog, "centralCell before:" << centralCellIndex[0] << ","  << centralCellIndex[1] << "," << centralCellIndex[2]);
		int idx = calcIndex(centralCellIndex, cellNums) + 1;	// 1D index of next cell
		// convert 1D to 3D index
		centralCellIndex[0] = idx % cellNums[0];
#if 1<DIM
		centralCellIndex[1] = (idx / cellNums[0]) % cellNums[1];
#endif
#if 3==DIM
		centralCellIndex[2] = idx / (cellNums[1] * cellNums[0]);
		//LOG4CXX_DEBUG(particlelog, "centralCell after:" << centralCellIndex[0] << ","  << centralCellIndex[1] << "," << centralCellIndex[2]);
#endif
		neighborCells = 0;

		/*
#if 3==DIM
		if (centralCellIndex[2]+1 < cellNums[2]) {
			centralCellIndex[2]++;
		}
		else
#endif
#if 1<DIM
		if (centralCellIndex[1]+1 < cellNums[1]) {
#endif
#if 3==DIM
			centralCellIndex[2] = 0;
#endif
#if 1<DIM
			centralCellIndex[1]++;
		}
		else
#endif
		if (centralCellIndex[0]+1 < cellNums[0]) {
#if 3==DIM
			centralCellIndex[2] = 0;
#endif
#if 1<DIM
			centralCellIndex[1] = 0;
#endif
			centralCellIndex[0]++;
		}
		*/
		selectCell(centralCellIndex);
	}
	//LOG4CXX_DEBUG(particlelog, "next is:" << particleIteratorInCell->p->getX().toString());
	return nextInCell();
}

bool ParticleContainerLC::hasNextInCell() {
	return particleIteratorInCell != NULL;
}
Particle& ParticleContainerLC::nextInCell() {
	Particle& p = *particleIteratorInCell->p;
	returnedParticleIteratorInCell = particleIteratorInCell;
	particleIteratorInCell = particleIteratorInCell->next;
	//LOG4CXX_DEBUG(particlelog, "next p in cell:" << p.getX().toString());
	return p;
}

void ParticleContainerLC::resetIterator() { //resetDomainIterator
	//LOG4CXX_DEBUG(particlelog, "reset");
	for (int i = 0; i < DIM; i++) {
		centralCellIndex[i] = 0;
	}
	neighborCells = 0;
	selectCell(centralCellIndex);
	//LOG4CXX_DEBUG(particlelog, "reseted");
}

bool ParticleContainerLC::hasNextOtherInCell() {
	if (otherCurrentCell == centralCell)
		return (otherParticleIteratorInCell != NULL) && (otherParticleIteratorInCell != returnedParticleIteratorInCell || returnedParticleIteratorInCell->next != NULL);
	else
		return otherParticleIteratorInCell != NULL;
}

Particle& ParticleContainerLC::nextOtherInCell() {
	Particle& p = *otherParticleIteratorInCell->p;
	otherParticleIteratorInCell = otherParticleIteratorInCell->next;
	if (otherParticleIteratorInCell == returnedParticleIteratorInCell)	// skip identical particle
		otherParticleIteratorInCell = otherParticleIteratorInCell->next;
	return p;
}

bool ParticleContainerLC::hasNextOther() {
	//LOG4CXX_DEBUG(particlelog, "hasNextOther");
	if (hasNextOtherInCell())
		return true;
	//LOG4CXX_DEBUG(particlelog, "not hasNextOtherInCell");
	int idx[DIM];
	/*for (int d = 0; d < DIM; d++) {
		idx[d] = otherCellIndex[d];
	}*/
	int other[DIM];
	numToIndex(calcIndex(otherCellIndex, allCellNums)+1, other, allCellNums);	// next cell
	for (idx[0] = other[0]; idx[0] < centralCellIndex[0]+3; idx[0]++)
#if 1<DIM
		for (idx[1] = other[1]; idx[1] < centralCellIndex[1]+3; idx[1]++)
#endif
#if 3==DIM
			for (idx[2] = other[2]; idx[2] < centralCellIndex[2]+3; idx[2]++)
#endif
			{
					if (allCells[calcIndex(idx, allCellNums)]->root != NULL) {
						if (allCells[calcIndex(idx, allCellNums)]->root != returnedParticleIteratorInCell)
							return true;
						if (returnedParticleIteratorInCell->next != NULL)
							return true;
					}
			}
	return false;	// passed all neighbor cells without finding particles

	/*
	while(true) {
		do {//LOG4CXX_DEBUG(particlelog, "in loop");
#if 3==DIM
			if (idx[2] - beginningOtherCellIndex[2] < 2) {
				idx[2]++;
			} else {
#endif
#if 1<DIM
				if (idx[1] - beginningOtherCellIndex[1] < 2) {
	#if 3==DIM
					idx[2] = beginningOtherCellIndex[2];
	#endif
					idx[1]++;
				} else {
#endif
					if (idx[0] - beginningOtherCellIndex[0] < 2) {
#if 3==DIM
						idx[2] = beginningOtherCellIndex[2];
#endif
#if 1<DIM
						idx[1] = beginningOtherCellIndex[1];
#endif
						idx[0]++;
					}
					else {
						LOG4CXX_DEBUG(particlelog, "passed all");
						return false;	// passed all surrounding (and empty) cells left
					}
				}
				LOG4CXX_DEBUG(particlelog, "other begin cell:" << beginningOtherCellIndex[0] << "," << beginningOtherCellIndex[1] << "," << beginningOtherCellIndex[2]);
				LOG4CXX_DEBUG(particlelog, "other cell:" << idx[0] << "," << idx[1] << "," << idx[2]);
				LOG4CXX_DEBUG(particlelog, "central cell:" << centralCellIndex[0]+1 << "," << centralCellIndex[1]+1 << "," << centralCellIndex[2]+1);
			}
		} while (allCells[calcIndex(idx, allCellNums)]->root == NULL);
		if (allCells[calcIndex(idx, allCellNums)]->root != returnedParticleIteratorInCell) {// skip particle got from next()
			//LOG4CXX_DEBUG(particlelog, "true:" << allCells[calcIndex(idx, allCellNums)]->root << "," << returnedParticleIteratorInCell);
			return true;
		}
		else if (returnedParticleIteratorInCell->next != NULL)	// cell has another particle
			return true;
		// else: go to next surrounding cell
	}
	return false;
	*/

	/*
	bool b = false;
	int j;
	for (int i = 0; i < DIM; i++) {
		b = ((centralCellIndex[i] == cellNums[i] - 1) || (centralCellIndex[i] == 0));	// centralCell at beginning or end of dim i
		if (b) {
			j = i;
		}
	}
	switch (DIM) {
	case 3:
		if (b) {
			if ((centralCellIndex[j] == 0) || centralCellIndex[j] == cellNums[j - 1]) {
				if (neighborCells < 7) {
					return true;
				}
			} else if (neighborCells < 11) {
				return true;
			}
		} else if (neighborCells < 14) {
			return true;
		} else
			return false;
		break;
	case 2:
		if (b) {
			if ((centralCellIndex[j] == 0) || centralCellIndex[j] == cellNums[j - 1]) {
				if (neighborCells < 3) {
					return true;
				}
			} else if (neighborCells < 5) {
				return true;
			}
		} else if (neighborCells < 8) {
			return true;
		} else
			return false;
		break;
	case 1:
		if (b) {
			if (neighborCells < 1) {
				return true;
			}
		} else if (neighborCells < 2) {
			return true;
		} else
			return false;
		break;
	}
	return false;*/
}
//(Solved by adding halo cells) PROBLEM:when centralCell becomes boundary cell
Particle & ParticleContainerLC::nextOther() { //cellIteration
	while (!hasNextOtherInCell()) {
		// switch to next cell
		if (otherCellIndex[0] < beginningOtherCellIndex[0]+2) {
			otherCellIndex[0]++;
		}
		else if (otherCellIndex[1] < beginningOtherCellIndex[1]+2) {
			otherCellIndex[1]++;
			otherCellIndex[0] = beginningOtherCellIndex[0];
			//otherCellIndex[2] = beginningOtherCellIndex[2];
		}
		else if (otherCellIndex[2] < beginningOtherCellIndex[2]+2) {
			otherCellIndex[2]++;
			otherCellIndex[0] = beginningOtherCellIndex[0];
			otherCellIndex[1] = beginningOtherCellIndex[1];
		}
		else {
			exit(-1);
		}

		int idx = calcIndex(otherCellIndex, allCellNums);
		//numToIndex(idx, otherCellIndex, allCellNums);	// calculate next cell 3D index

		otherCurrentCell = allCells[idx];
		otherParticleIteratorInCell = otherCurrentCell->root;
		neighborCells++;

		//LOG4CXX_DEBUG(particlelog, "other cell:" << otherCellIndex[0] << "," << otherCellIndex[1] << "," << otherCellIndex[2]);
		//LOG4CXX_DEBUG(particlelog, "central cell:" << centralCellIndex[0]+1 << "," << centralCellIndex[1]+1 << "," << centralCellIndex[2]+1);

		/*
		if (otherCellIndex[2] - beginningOtherCellIndex[2] < 2) {
			otherCellIndex[2]++;
		} else {
			if (otherCellIndex[1] - beginningOtherCellIndex[1] < 2) {
				otherCellIndex[2] = beginningOtherCellIndex[2];
				otherCellIndex[1]++;
			} else {
				if (otherCellIndex[0] - beginningOtherCellIndex[0] < 2) {
					otherCellIndex[2] = beginningOtherCellIndex[2];
					otherCellIndex[1] = beginningOtherCellIndex[1];
					otherCellIndex[0]++;
				}
				else
					return nextOtherInCell();	// error
			}
		}
		otherCurrentCell = allCells[calcIndex(otherCellIndex, allCellNums)];
		otherParticleIteratorInCell = otherCurrentCell->root;
		neighborCells++;*/
	}
	if (otherParticleIteratorInCell == returnedParticleIteratorInCell)	// skip identical particle
		otherParticleIteratorInCell = otherParticleIteratorInCell->next;
	return nextOtherInCell();
}

bool ParticleContainerLC::hasNextBoundary3D() {
	return ((boundaryCurrent != 2 * DIM) || (hasNextBoundaryWall()));
}
Particle & ParticleContainerLC::nextBoundary3D() {
	Particle p;
	if (boundaryCurrent == 0) {
		xc = 0;
		yc = 1;
		p = nextBoundaryWall(2, 0);
	} else if (boundaryCurrent == 1) {
		xc = 0;
		yc = 1;
		p = nextBoundaryWall(2, cellNums[2]);
	} else if (boundaryCurrent == 2) {
		xc = 0;
		yc = 2;
		p = nextBoundaryWall(1, 0);
	} else if (boundaryCurrent == 3) {
		xc = 0;
		yc = 2;
		p = nextBoundaryWall(1, cellNums[1]);
	} else if (boundaryCurrent == 4) {
		xc = 1;
		yc = 2;
		p = nextBoundaryWall(0, 0);
	} else if (boundaryCurrent == 5) {
		xc = 1;
		yc = 2;
		p = nextBoundaryWall(0, cellNums[0]);
	}
	if (!hasNextBoundaryWall()) {
		if (boundaryCurrent < 2 * DIM) {
			boundaryCurrent++;
		}
	}
	return p;
}

bool ParticleContainerLC::hasNextBoundaryWall() {
	return (((boundaryIndex[xc] < cellNums[xc]) || (boundaryIndex[yc] < cellNums[yc]))
			|| (hasNextInCell()));
}
Particle& ParticleContainerLC::nextBoundaryWall(int fixed,
		int fixedValue) {
	Particle p;
	if (!hasNextInCell()) {
		if (boundaryIndex[xc] < cellNums[xc]) {
			boundaryIndex[xc]++;
		} else {
			if (boundaryIndex[yc] < cellNums[yc]) {
				boundaryIndex[xc] = 0;
				boundaryIndex[yc]++;
			} else {
				return p;
			}
		}
		boundaryIndex[fixed] = fixedValue;
		selectCell(boundaryIndex);
	}
	p = nextInCell();
	return p;
}
void ParticleContainerLC::resetBoundaryIterator() {
	for (int i = 0; i < DIM; i++) {
		boundaryIndex[i] = 0;
	}
	selectCell(boundaryIndex);
	xc = 0;
	yc = 0;
}

bool ParticleContainerLC::hasNextBoundary2D() {
	return ((boundaryIndex[0] < cellNums[0]) || (boundaryIndex[1] < cellNums[1]));
}

Particle& ParticleContainerLC::nextBoundary2D() {
	Particle p;
	if (!hasNextInCell()) {
		bool b1, b2;
		b1 = (boundaryIndex[0] != 0) && (boundaryIndex[0] != cellNums[0] - 1);
		while ((b1) && (boundaryIndex[0] < cellNums[0])) {
			boundaryIndex[0]++;
			b2 = (boundaryIndex[1] != 0) && (boundaryIndex[1] != cellNums[1] - 1);
			while (((b2) && (b1)) && (boundaryIndex[1] < cellNums[1])) {
				boundaryIndex[1]++;
			}
		}
		selectCell(boundaryIndex);
		p = nextInCell();
		return p;
	} else
		p = nextInCell();
	return p;
}

Particle& ParticleContainerLC::nextBoundary() {
#if 3==DIM
	return nextBoundary3D();
#elif 2==DIM
	return nextBoundary2D();
#endif
	/*#elif 1==DIM
	 return next();
	 #endif*/
}

bool ParticleContainerLC::hasNextBoundary() {
#if 3==DIM
	return hasNextBoundary3D();
#elif 2==DIM
	return hasNextBoundary2D();
#endif
	/*#elif 1==DIM
	 return hasNext();
	 #endif*/
}

bool ParticleContainerLC::hasNextHaloParticle(){
	return (hasNextInCell()||haloIndex<haloAllCellNums);
}

Particle& ParticleContainerLC::nextHaloParticle() {
	Particle p;
	if (!hasNextInHaloCell()) {
		if (haloIndex < haloAllCellNums) {
			currentHaloCell = *haloCells[haloIndex].root;
			haloIndex++;
			haloIteratorInCell = &currentHaloCell;
			p = nextInCell();
			return p;
		}
	}
	p = nextInCell();
	return p;
}

bool ParticleContainerLC::hasNextInHaloCell() {
	return haloIteratorInCell != NULL;
}

Particle& ParticleContainerLC::nextInHaloCell() {
	Particle& p = *haloIteratorInCell->p;
	haloIteratorInCell = haloIteratorInCell->next;
	return p;
}

//PROBLEM : we should make it with X coordinates, but here we don't know where the seperate grids begin
void ParticleContainerLC::addHalo(Particle& p, int i) {
	particles.push_back(p);
	resetIterator();
	ParticleList pl;
		pl.p = &p;
		insertList((ParticleList**)&haloCells[i], &pl);
/*#if 1==DIM
	int particleIndex[]= {(p.getX()[0])/radius-1};
#elif 2==DIM
	int particleIndex[]= {(p.getX()[0]/radius-1),(p.getX()[1]/radius-1)};
#elif 3==DIM
	int particleIndex[] = { (p.getX()[0] / radius - 1), (p.getX()[1] / radius
			- 1), (p.getX()[2] / radius - 1) };
#endif
	bool b = true;
	for (int i = 0; i < DIM; i++) {
		b = b && ((particleIndex[i] == 0) || (particleIndex[i] == cellNums[i]));
	}
	if (b) {
		int haloIndexTmp;
		if (DIM == 2) {
			if (particleIndex[0] == -1) {
				haloIndexTmp = cellNums[2] - particleIndex[2];
			} else if (particleIndex[1] == cellNums[0]) {
				haloIndexTmp = cellNums[2] * particleIndex[1]
						+ particleIndex[0];
			}
		}
		ParticleList pl;
		pl.p = p;
		insertList(&(haloCells[haloIndexTmp]), &pl);
	} else
		error();*/

}

void ParticleContainerLC::removeNextHalo() {
	ParticleList pl;
	pl.p = &nextHaloParticle();
	deleteList((ParticleList**)&pl);
	//particles.remove(p); that would destroy the particle forever
}

void ParticleContainerLC::removeHaloParticle(Particle& p){
	Particle pTmp=next();
	while(&pTmp!=&p){
		pTmp=next();
	}
	ParticleList pl;
	pl.p = &nextHaloParticle();
	deleteList((ParticleList**)&pl);
	//particles.remove(p); that would destroy the particle forever
}

void ParticleContainerLC::resetHaloIterator() {
	haloIndex = 0;
	currentHaloCell = *haloCells[0].root;
}

void ParticleContainerLC::emptyHalo() {
	//resetHaloIterator();
	for (int i = 0; i < haloAllCellNums; i++) {
		//LOG4CXX_DEBUG(particlelog, "empty halo i" << i << " of " << haloAllCellNums);
		/*ParticleList *q = haloCells[i].root;
		while (q != NULL) {
			q = q->next;

		}*/
		haloCells[i].root = NULL;
	}
}

void ParticleContainerLC::numToIndex(int idx, int arr[], int size[]) {
	arr[0] = idx % size[0];
#if 1<DIM
	arr[1] = (idx / size[0]) % size[1];
#endif
#if 3==DIM
	arr[2] = idx / (size[1] * size[0]);
#endif
}



void ParticleContainerLC::iterate(PCApply *fnc) {
	for (int i = 0; i < numcell(cellNums); i++) {
		ParticleList *pl = cells[i].root;
		while (pl != NULL) {
			Particle& p = *pl->p;
			fnc->iterateFunc(p);
			//LOG4CXX_DEBUG(particlelog, "iterate");
			pl = pl->next;
		}
	}
}

void ParticleContainerLC::iteratePair(PCApply *fnc) {
	for (int i = 0; i < numcell(cellNums); i++) {
		//LOG4CXX_DEBUG(particlelog, "i:" << i << " of " << numcell(cellNums));
		ParticleList *pl = cells[i].root;
		int idx[DIM];
		numToIndex(i, idx, cellNums);
		selectCell(idx);
		while (pl != NULL) {
			int tmp[DIM];
			int other[DIM];
			numToIndex(calcIndex(otherCellIndex, allCellNums), other, allCellNums);	// next cell
			int count = 0;
			for (tmp[0] = other[0]; tmp[0] < idx[0]+3; tmp[0]++)	// +1 for centralCellIndex to allCells coords.
#if 1<DIM
				for (tmp[1] = other[1]; tmp[1] < idx[1]+3; tmp[1]++)
#endif
#if 3==DIM
					for (tmp[2] = other[2]; tmp[2] < idx[2]+3; tmp[2]++)
#endif
					{
						count++;
						ParticleList *pl2 = allCells[calcIndex(tmp, allCellNums)]->root;
						while (pl2 != NULL) {
							if (pl != pl2) {
								Particle& p1 = *pl->p;
								Particle& p2 = *pl2->p;
								fnc->iteratePairFunc(p1, p2);
							}
							pl2 = pl2->next;
						}
					}
			//LOG4CXX_DEBUG(particlelog, "others count:" << count);
			pl = pl->next;
		}
	}
}
