/*
 * ParticleContainerForLCAlg.cpp
 *
 *  Created on: 16.11.2014
 *      Author: vmuser
 */

#include "ParticleContainerForLCAlg.h"

ParticleContainerForLCAlg::ParticleContainerForLCAlg() {
	// TODO Auto-generated constructor stub

}

ParticleContainerForLCAlg::~ParticleContainerForLCAlg() {
	// TODO Auto-generated destructor stub
}

ParticleContainerForLCAlg::ParticleContainerForLCAlg(double radius,
		double * domainSize, const ParticleContainer& pc) {
	//NOTE:compiles if particles,iterator and othersIterator from ParticleContainer are public
	particles = pc.particles;
	iterator = pc.iterator;
	othersIterator = pc.othersIterator;
#if 1==DIM
	int cellNums[]= {(domainSize[0]/radius)};
#elif 2==DIM
	int cellNums[]= {(domainSize[0]/radius),(domainSize[1]/radius)};
	int haloNums[]= {(cellNums[0]+2),(cellNums[1]+2)};
	halloAllCellNums=2*((cellNums[0]+2)+(cellNums[1])+2);
#elif 3==DIM
	int cellNums[] = { (int) (domainSize[0] / radius), (int) (domainSize[1]
			/ radius), (int) (domainSize[2] / radius) };
	int haloNums[] = { (cellNums[0] + 2), (cellNums[1] + 2), (cellNums[2]
			+ 2) };
	halloAllCellNums = 2
			* (((cellNums[0] + 2) * (cellNums[1] + 2))
					+ (cellNums[1] * cellNums[2] + 2)
					+ (cellNums[0] * cellNums[2] + 2));
#endif
	cells = new ParticleList[numcell(cellNums)];
	haloCells = new ParticleList[halloAllCellNums];
	int i = 0;
	while (hasNext()) {
		Particle p = next();
#if 1==DIM
		int particleIndex[]= {p.getX()[0] / radius};
#elif 2==DIM
		int particleIndex[]= {p.getX()[0] / radius, p.getX()[1] / radius};
#elif 3==DIM
		int particleIndex[] = { p.getX()[0] / radius, p.getX()[1] / radius,
				p.getX()[2] / radius };
#endif
		ParticleList pl;
		pl.p = p;
		insertList(&(cells[calcIndex(particleIndex, cellNums)]), &pl);
	}
	resetIterator();
	resetBoundaryIterator();
	resetHaloIterator();
}
//PROBLEM:we don't know where the seperate grids begin: p.getX()[0]/radius is not correct there is also shift with 20 or 70 for our example
void ParticleContainerForLCAlg::add(Particle& p) {
	particles.push_back(p);
	resetIterator();
#if 1==DIM
	int particleIndex[]= {p.getX()[0]/radius};
#elif 2==DIM
	int particleIndex[]= {p.getX()[0]/radius,p.getX()[1]/radius};
#elif 3==DIM
	int particleIndex[] = { p.getX()[0] / radius, p.getX()[1] / radius,
			p.getX()[2] / radius };
#endif
	ParticleList pl;
	pl.p = p;
	insertList(&(cells[calcIndex(particleIndex, cellNums)]), &pl);
}

void ParticleContainerForLCAlg::selectCell(int celli[]) {
	for (int i = 0; i < DIM; i++) {
		beginningOtherCellIndex[i] = celli[i] - 1;
		if (beginningOtherCellIndex[i] < 0) {
			beginningOtherCellIndex[i] = 0;
		}
		otherCellIndex[i] = beginningOtherCellIndex[i];
	}
	otherCurrentCell = cells[calcIndex(beginningOtherCellIndex, cellNums)];
	otherParticleIteratorInCell = &otherCurrentCell;
	centralCell = cells[calcIndex(celli, cellNums)];
	particleIteratorInCell = &centralCell;
}

void ParticleContainerForLCAlg::insertList(ParticleList *list,
		ParticleList *i) {
	i->next = list;
	list = i;
}

void ParticleContainerForLCAlg::deleteList(ParticleList *q) {
	q = (q)->next;
}

void ParticleContainerForLCAlg::moveParticles_LC(ParticleList cells[], int *nc,
		double *l) {
	int dim = DIM;
	int ic[dim], kc[dim];
	for (ic[0] = 0; ic[0] < nc[0]; ic[0]++)
		for (ic[1] = 0; ic[1] < nc[1]; ic[1]++)
#if 3==DIM
			for (ic[2] = 0; ic[2] < nc[2]; ic[2]++)
#endif
					{
				ParticleList *q = &cells[calcIndex(ic, nc)];
				ParticleList *i = q;
				while (NULL != i) {
					for (int d = 0; d < dim; d++)
						kc[d] = (int) floor(i->p.getX()[d] * nc[d] / l[d]);
					if ((ic[0] != kc[0]) || (ic[1] != kc[1])
#if 3==DIM
							|| (ic[2] != kc[2])
#endif
							) {
						deleteList(q);
						insertList((&cells[calcIndex(kc, nc)]), i);
					} else
						q = i->next;
					i = q;
				}
			}
}

bool ParticleContainerForLCAlg::hasNext() {
	bool b = true;
	for (int i = 0; i < DIM; i++) {
		b = b && (centralCellIndex[i] < cellNums[i]);
	}
	return b;
}
Particle& ParticleContainerForLCAlg::next() { //nextInDomain
	Particle p;
	if (!hasNextInCell()) {
#if 3==DIM
		if (centralCellIndex[2] < cellNums[2]) {
			centralCellIndex[2]++;
		}
#endif
#if 1<DIM
		if (centralCellIndex[1] < cellNums[1]) {
#endif
#if 3==DIM
			centralCellIndex[2] = 0;
#endif
#if 1<DIM
			centralCellIndex[1]++;
		}
#endif
		if (centralCellIndex[0] < cellNums[0]) {
#if 3==DIM
			centralCellIndex[2] = 0;
#endif
#if 1<DIM
			centralCellIndex[1] = 0;
#endif
			centralCellIndex[0]++;
		} else {
			return p;
		}
	}
	selectCell(centralCellIndex);
	p = nextInCell();
	return p;
}

bool ParticleContainerForLCAlg::hasNextInCell() {
	return particleIteratorInCell != NULL;
}
Particle& ParticleContainerForLCAlg::nextInCell() {
	Particle& p = particleIteratorInCell->p;
	particleIteratorInCell = particleIteratorInCell->next;
	return p;
}

void ParticleContainerForLCAlg::resetIterator() { //resetDomainIterator
	for (int i = 0; i < DIM; i++) {
		centralCellIndex[i] = 0;
	}
	NeighborCells = 0;
	selectCell(centralCellIndex);
}

bool ParticleContainerForLCAlg::hasNextOtherInCell() {
	return otherParticleIteratorInCell != NULL;
}
Particle& ParticleContainerForLCAlg::nextOtherInCell() {
	Particle& p = otherParticleIteratorInCell->p;
	otherParticleIteratorInCell = otherParticleIteratorInCell->next;
	return p;
}

bool ParticleContainerForLCAlg::hasNextOther() {
	bool b = false;
	int j;
	for (int i = 0; i < DIM; i++) {
		b = ((centralCellIndex[i] == cellNums[i] - 1) || (centralCellIndex[i] == 0));
		if (b) {
			j = i;
		}
	}
	switch (DIM) {
	case 3:
		if (b) {
			if ((centralCellIndex[j] == 0) || centralCellIndex[j] == cellNums[j - 1]) {
				if (NeighborCells < 7) {
					return true;
				}
			} else if (NeighborCells < 11) {
				return true;
			}
		} else if (NeighborCells < 14) {
			return true;
		} else
			return false;
		break;
	case 2:
		if (b) {
			if ((centralCellIndex[j] == 0) || centralCellIndex[j] == cellNums[j - 1]) {
				if (NeighborCells < 3) {
					return true;
				}
			} else if (NeighborCells < 5) {
				return true;
			}
		} else if (NeighborCells < 8) {
			return true;
		} else
			return false;
		break;
	case 1:
		if (b) {
			if (NeighborCells < 1) {
				return true;
			}
		} else if (NeighborCells < 2) {
			return true;
		} else
			return false;
		break;
	}
}
//PROBLEM:when centralCell becomes boundary cells
Particle & ParticleContainerForLCAlg::nextOther() { //cellIteration
	Particle p;
	do {
		if (!hasNextOtherInCell()) {
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
					} else {
						return p;
					}
				}
			}
			otherCurrentCell = cells[calcIndex(otherCellIndex, cellNums)];
			particleIteratorInCell = &otherCurrentCell;
		}
		p = nextOtherInCell();
	} while (p == particleIteratorInCell->p);
	return p;
}

bool ParticleContainerForLCAlg::hasNextBoundary3D() {
	return ((boundaryCurrent != 2 * DIM) || (hasNextBoundaryWall()));
}
Particle & ParticleContainerForLCAlg::nextBoundary3D() {
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

bool ParticleContainerForLCAlg::hasNextBoundaryWall() {
	return (((boundaryIndex[xc] < cellNums[xc]) || (boundaryIndex[yc] < cellNums[yc]))
			|| (hasNextInCell()));
}
Particle& ParticleContainerForLCAlg::nextBoundaryWall(int fixed,
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
void ParticleContainerForLCAlg::resetBoundaryIterator() {
	for (int i = 0; i < DIM; i++) {
		boundaryIndex[i] = 0;
	}
	selectCell(boundaryIndex);
	xc = 0;
	yc = 0;
}

bool ParticleContainerForLCAlg::hasNextBoundary2D() {
	return ((boundaryIndex[0] < cellNums[0]) || (boundaryIndex[1] < cellNums[1]));
}

Particle& ParticleContainerForLCAlg::nextBoundary2D() {
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

Particle& ParticleContainerForLCAlg::nextBoundary() {
#if 3==DIM
	return nextBoundary3D();
#elif 2==DIM
	return nextBoundary2D();
#endif
	/*#elif 1==DIM
	 return next();
	 #endif*/
}

bool ParticleContainerForLCAlg::hasNextBoundary() {
#if 3==DIM
	return hasNextBoundary3D();
#elif 2==DIM
	return hasNextBoundary2D();
#endif
	/*#elif 1==DIM
	 return hasNext();
	 #endif*/
}

bool ParticleContainerForLCAlg::hasNextHaloParticle(){
	return (hasNextInCell()||haloIndex<halloAllCellNums);
}

Particle& ParticleContainerForLCAlg::nextHaloParticle() {
	Particle p;
	if (!hasNextInCell()) {
		if (haloIndex < halloAllCellNums) {
			currentHaloCell = haloCells[haloIndex];
			haloIndex++;
			haloIteratorInCell = &currentHaloCell;
			p = nextInCell();
			return p;
		}
	}
	p = nextInCell();
	return p;
}

bool ParticleContainerForLCAlg::hasNextInHaloCell() {
	return haloIteratorInCell != NULL;
}

Particle& ParticleContainerForLCAlg::nextInHaloCell() {
	Particle& p = haloIteratorInCell->p;
	haloIteratorInCell = haloIteratorInCell->next;
	return p;
}

//PROBLEM : we should make it with X coordinates, but here we don't know where the seperate grids begin
void ParticleContainerForLCAlg::addHalo(Particle& p, int i) {
	particles.push_back(p);
	resetIterator();
	ParticleList pl;
		pl.p = p;
		insertList(&(haloCells[i]), &pl);
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

void ParticleContainerForLCAlg::removeNextHalo() {
	ParticleList pl;
	pl.p=nextHaloParticle();
	deleteList(&pl);
	//particles.remove(p); that would destroy the particle forever
}

void ParticleContainerForLCAlg::removeHaloParticle(Particle& p){
	Particle pTmp=next();
	while(&pTmp!=&p){
		pTmp=next();
	}
	ParticleList pl;
	pl.p=nextHaloParticle();
	deleteList(&pl);
	//particles.remove(p); that would destroy the particle forever
}

void ParticleContainerForLCAlg::resetHaloIterator() {
	haloIndex = 0;
	currentHaloCell = haloCells[0];
}
