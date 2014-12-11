/*
 * ParticleContainerLC.cpp
 *
 *  Created on: 16.11.2014
 *      Author: vmuser
 */

#include "ParticleContainerLC.h"
#include "Simulation.h"
#include "Logger.h"

const LoggerPtr particlelog(Logger::getLogger("molsim.particle"));

ParticleContainerLC::ParticleContainerLC() {
	// TODO Auto-generated constructor stub

}

ParticleContainerLC::~ParticleContainerLC() {
	delete [] cells;
	delete [] haloCells;
	delete [] allCells;
}

ParticleContainerLC::ParticleContainerLC(ParticleContainer* pc, Simulation *sim) {
	for (int i = 0; i < 3; i++) {
		domainSize[i] = sim->domainSize[i];
	}
	radius = sim->cutoff;
	distance = sim->meshWidth;
	for (int i = 0; i < 6; i++) {
		domainBoundary[i] = sim->boundaries->getBoundary((BoundaryConds::Side)i);
	}

	for (int d = 0; d < DIM; d++) {
		cellSize[d] = domainSize[d] / radius;
		int expansion = domainSize[d] / radius;
		cellNums[d] = expansion > 0 ? expansion : 1;	// expansion in dimension d must be at least 1 for correct numcell()
		allCellNums[d] = cellNums[d] + 2;
	}

#if 1==DIM
	allCellNums[0] = cellNums[0] + 2;
#elif 2==DIM
	haloAllCellNums= (cellNums[0] + 2) * (cellNums[1] + 2) - cellNums[0]*cellNums[1];
#elif 3==DIM
	haloAllCellNums = (cellNums[0] + 2) * (cellNums[1] + 2) * (cellNums[2] + 2) - cellNums[0]*cellNums[1]*cellNums[2];
#endif
	cells = new Cell[numcell(cellNums)];
	haloCells = new Cell[haloAllCellNums];
	allCells = new Cell*[numcell(allCellNums)];

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
	//LOG4CXX_DEBUG(particlelog, "before insert");

	if (pc != NULL) {
		particles = pc->particles;
		iterator = pc->iterator;
		othersIterator = pc->othersIterator;

		pc->resetIterator();
		while (pc->hasNext()) {
			Particle& p = pc->next();
#if 1==DIM
			int particleIndex[]= {p.getX()[0] / radius, 0, 0};
#elif 2==DIM
			int particleIndex[]= {p.getX()[0] / radius, p.getX()[1] / radius, p.getX()[2] / radius};
#elif 3==DIM
			int particleIndex[] = { p.getX()[0] / radius, p.getX()[1] / radius,
					p.getX()[2] / radius };
#endif
			// skip particles that don't suite into the domain
			//LOG4CXX_DEBUG(particlelog, "add p: " << p.getX().toString() << ", domain:" << domainSize[0] << "," << domainSize[1] << "," << domainSize[2]);
			bool outofbound = false;
			for (int d = 0; d < 3; d++) {
				if (particleIndex[d] >= cellNums[d] || particleIndex[d] < 0
						|| p.getX()[d] < 0.0 || p.getX()[d] > domainSize[d])
					outofbound = true;
			}
			if (outofbound)
				continue;

			count++;
			ParticleList* pl = new ParticleList;
			pl->p = &p;
			pl->next = NULL;
			insertList((ParticleList**)&(cells[calcIndex(particleIndex, cellNums)].root), pl);
			//LOG4CXX_DEBUG(particlelog, "add p: " << p.getX().toString() << " to index " << particleIndex[0] << "," << particleIndex[1] << "," << particleIndex[2]);
			//LOG4CXX_DEBUG(particlelog, "root: " << cells[calcIndex(particleIndex, cellNums)].root->p->getX().toString());
		}
	}
	LOG4CXX_DEBUG(particlelog, "grid: " << cellNums[0] << " x " << cellNums[1] <<
		#if 3==DIM
			" x " << cellNums[2] <<
		#endif
			", inserted count:" << count << ", size() delivers:" << size());
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

void ParticleContainerLC::add(Particle& p) {
	particles.push_back(p);
	resetIterator();

	// define index of cell for particle
#if 1==DIM
	int particleIndex[]= {p.getX()[0]/radius, p.getX()[1]/radius, p.getX()[2] / radius};
#elif 2==DIM
	int particleIndex[]= {p.getX()[0]/radius, p.getX()[1]/radius, p.getX()[2] / radius};
#elif 3==DIM
	int particleIndex[] = { p.getX()[0] / radius, p.getX()[1] / radius,
			p.getX()[2] / radius };
#endif

	// skip particles that don't suite into the domain
	bool outofbound = false;
	for (int d = 0; d < 3; d++) {
		if (particleIndex[d] >= cellNums[d] || particleIndex[d] < 0
				|| p.getX()[d] < 0.0 || p.getX()[d] > domainSize[d])
			outofbound = true;
	}
	if (outofbound)
		return;

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

void ParticleContainerLC::resetIterator() { //resetDomainIterator
	for (int i = 0; i < DIM; i++) {
		centralCellIndex[i] = 0;
	}
	neighborCells = 0;
	selectCell(centralCellIndex);
	//LOG4CXX_DEBUG(particlelog, "reseted");
}

Particle& ParticleContainerLC::next() { //nextInDomain
	while (!hasNextInCell()) {	// switch to next cell if no particles left in centralCell
		int idx = calcIndex(centralCellIndex, cellNums) + 1;	// 1D index of next cell
		numToIndex(idx, centralCellIndex, cellNums);	// convert 1D to 3D index
		neighborCells = 0;
		selectCell(centralCellIndex);
	}
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
	for (ic[0] = 1; ic[0] < nc[0]-1; ic[0]++)
		for (ic[1] = 1; ic[1] < nc[1]-1; ic[1]++)
#if 3==DIM
			for (ic[2] = 1; ic[2] < nc[2]-1; ic[2]++)
#endif
					{
				//LOG4CXX_DEBUG(particlelog, "move() index: " << calcIndex(ic, nc) << " of " << numcell(allCellNums));
				ParticleList **q = &cell_arr[calcIndex(ic, nc)]->root;	// get cell/ first ParticleList at index ic
				ParticleList *i = *q;
				//LOG4CXX_DEBUG(particlelog, "got i in move():" << q);
				while (NULL != i) {
					Particle& p = *i->p;
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

void ParticleContainerLC::emptyHalo() {
	//resetHaloIterator();
	for (int i = 0; i < haloAllCellNums; i++) {
		//LOG4CXX_DEBUG(particlelog, "empty halo i" << i << " of " << haloAllCellNums);
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
			//LOG4CXX_DEBUG(particlelog, "iterate" << p.getX().toString());
			pl = pl->next;
		}
	}
	//LOG4CXX_DEBUG(particlelog, "iterate end");
}

void ParticleContainerLC::iteratePair(PCApply *fnc) {
	//LOG4CXX_DEBUG(particlelog, "iterate pair");
	for (int i = 0; i < numcell(cellNums); i++) {
		//LOG4CXX_DEBUG(particlelog, "cell " << i << " of " << numcell(cellNums));
		ParticleList *pl = cells[i].root;
		int idx[DIM];
		numToIndex(i, idx, cellNums);
		//LOG4CXX_DEBUG(particlelog, "idx: " << idx[0] << "," << idx[1] << "," << idx[2]);
		selectCell(idx);
		while (pl != NULL) {
			int tmp[DIM];
			//int other[DIM];
			//numToIndex(calcIndex(otherCellIndex, allCellNums), other, allCellNums);	// next cell
			for (tmp[0] = otherCellIndex[0]; tmp[0] < idx[0]+3; tmp[0]++)	// incl. +1 for centralCellIndex to allCells coords.
#if 1<DIM
				for (tmp[1] = otherCellIndex[1]; tmp[1] < idx[1]+3; tmp[1]++)
#endif
#if 3==DIM
					for (tmp[2] = otherCellIndex[2]; tmp[2] < idx[2]+3; tmp[2]++)
#endif
					{
						ParticleList *pl2 = allCells[calcIndex(tmp, allCellNums)]->root;
						while (pl2 != NULL) {
							if (pl != pl2) {
								Particle& p1 = *pl->p;
								Particle& p2 = *pl2->p;
								Particle p(p2);
								//LOG4CXX_DEBUG(particlelog, p1.getX().toString());
								// correct position of halo particles for periodic boundaries
								for (int d = 0; d < DIM; d++) {
									utils::Vector<double, 3>& x = p.getX();
									if (cellNums[d] > 1) {
										if (tmp[d] == 0)
											x[d] -= radius*cellNums[d];
										else if (tmp[d] == cellNums[d])
											x[d] += radius*cellNums[d];
									}
								}
								//LOG4CXX_DEBUG(particlelog, "p:" << p.getX().toString());
								//LOG4CXX_DEBUG(particlelog, "p2" << p2.getX().toString());

								fnc->iteratePairFunc(p1, p);
								//LOG4CXX_DEBUG(particlelog, "f after:" << p1.getF().toString());
							}
							pl2 = pl2->next;
						}
					}
			pl = pl->next;
		}
	}
}

void ParticleContainerLC::applyBoundaryConds(BoundaryConds::Boundary bd, PCApply *fnc) {
	if (bd == BoundaryConds::REFLECTING) {	// fnc should calculate force
		if (domainBoundary[BoundaryConds::LEFT] == BoundaryConds::REFLECTING) {	// left
			applyToBoundaryWall(0, 0, fnc);
		}
		if (domainBoundary[BoundaryConds::RIGHT] == BoundaryConds::REFLECTING) {	// right
			applyToBoundaryWall(0, cellNums[0]-1, fnc);
		}

#if 1<DIM
		if (domainBoundary[BoundaryConds::BOTTOM] == BoundaryConds::REFLECTING) {	// bottom
			applyToBoundaryWall(1, 0, fnc);
		}
		if (domainBoundary[BoundaryConds::TOP] == BoundaryConds::REFLECTING) {	// top
			applyToBoundaryWall(1, cellNums[1]-1, fnc);
		}
#endif

#if 3==DIM
		if (domainBoundary[BoundaryConds::FRONT] == BoundaryConds::REFLECTING) {	// front
			applyToBoundaryWall(2, 0, fnc);
		}
		if (domainBoundary[BoundaryConds::BACK] == BoundaryConds::REFLECTING) {	// back
			applyToBoundaryWall(2, cellNums[2]-1, fnc);
		}
#endif
	}
	else if (bd == BoundaryConds::PERIODIC) {
		if ((domainBoundary[BoundaryConds::LEFT] == BoundaryConds::PERIODIC) &&
				(domainBoundary[BoundaryConds::RIGHT] == BoundaryConds::PERIODIC)){	// left and right
			bindOppositeWalls(0, 0);
			bindOppositeWalls(0, allCellNums[0]-1);
		}
#if 1<DIM
		if ((domainBoundary[BoundaryConds::BOTTOM] == BoundaryConds::PERIODIC) &&
				(domainBoundary[BoundaryConds::TOP] == BoundaryConds::PERIODIC)){	// bottom and top
			bindOppositeWalls(1, 0);
			bindOppositeWalls(1, allCellNums[1]-1);
		}
#endif
#if 3==DIM
		if ((domainBoundary[BoundaryConds::FRONT] == BoundaryConds::PERIODIC) &&
				(domainBoundary[BoundaryConds::BACK] == BoundaryConds::PERIODIC)){	// front and backt
			bindOppositeWalls(2, 0);
			bindOppositeWalls(2, allCellNums[2]-1);
		}
#endif
	}
	else if (bd == BoundaryConds::OUTFLOW) {
		if (domainBoundary[BoundaryConds::LEFT] == BoundaryConds::OUTFLOW) {	// left
			emptyHaloSide(0, 0);
		}
		if (domainBoundary[BoundaryConds::RIGHT] == BoundaryConds::OUTFLOW) {	// right
			emptyHaloSide(0, cellNums[0]-1);
		}

		if (domainBoundary[BoundaryConds::BOTTOM] == BoundaryConds::OUTFLOW) {	// bottom
			emptyHaloSide(1, 0);
		}
		if (domainBoundary[BoundaryConds::TOP] == BoundaryConds::OUTFLOW) {	// top
			emptyHaloSide(1, cellNums[1]-1);
		}

		if (domainBoundary[BoundaryConds::FRONT] == BoundaryConds::OUTFLOW) {	// front
			emptyHaloSide(2, 0);
		}
		if (domainBoundary[BoundaryConds::BACK] == BoundaryConds::OUTFLOW) {	// back
			emptyHaloSide(2, cellNums[2]-1);
		}
	}
}

void ParticleContainerLC::applyToBoundaryWall(int fixedDim, int fixedVal, PCApply *fnc) {
	int idx[DIM];
	idx[fixedDim] = fixedVal;
#if 1<DIM
	const int nextDim = (fixedDim+1) % DIM;
#endif
#if 3==DIM
	const int lastDim = (fixedDim+2) % DIM;
#endif

	// iterate over wall cells
#if 1<DIM
	for (idx[nextDim] = 0; idx[nextDim] < cellNums[nextDim]; idx[nextDim]++)
#endif
#if 3==DIM
		for (idx[lastDim] = 0; idx[lastDim] < cellNums[lastDim]; idx[lastDim]++)
#endif
		{
			ParticleList *pl = cells[calcIndex(idx, cellNums)].root;
			while (pl != NULL) {	// iterate over all particles in the cell
				Particle& p = *pl->p;
				utils::Vector<double, 3>& x = p.getX();
				if (fixedVal == 0) {
					if (x[fixedDim] < distance) {
						Particle ghost = p;
						utils::Vector<double, 3>& x2 = ghost.getX();
						x2[fixedDim] = 0.0;
						fnc->iteratePairFunc(p, ghost);
					}
				}
				else if (/*domainSize[fixedDim]*/radius*cellNums[fixedDim] - x[fixedDim] < distance) {
					Particle ghost = p;
					utils::Vector<double, 3>& x2 = ghost.getX();
					x2[fixedDim] = radius*cellNums[fixedDim];//domainSize[fixedDim];
					fnc->iteratePairFunc(p, ghost);
				}
				pl = pl->next;
			}
		}
}

//looks like it works by debugging, not by running the whole programm, to remove if another version exists
void ParticleContainerLC::bindOppositeWalls(int fixedDim, int fixedVal) {
	//LOG4CXX_DEBUG(particlelog, "start periodic");
	int idx[DIM];
	idx[fixedDim] = fixedVal;
#if 1<DIM
	const int nextDim = (fixedDim+1) % DIM;
#endif
#if 3==DIM
	const int lastDim = (fixedDim+2) % DIM;
#endif
	// iterate over wall cells
#if 1<DIM
	for (idx[nextDim] = 0; idx[nextDim] < allCellNums[nextDim]; idx[nextDim]++)
#endif
#if 3==DIM
		for (idx[lastDim] = 0; idx[lastDim] < allCellNums[lastDim]; idx[lastDim]++)
#endif
		{
			int index = calcIndex(idx, allCellNums);
			ParticleList **pl = &allCells[index]->root;
			ParticleList *i = *pl;

			// set index of opposite boundary wall
			int idx_n[DIM];
			for (int i = 0; i < DIM; i++)
				idx_n[i] = idx[i];
			if (fixedVal == 0)
				idx_n[fixedDim] = allCellNums[fixedDim]-2;
			else
				idx_n[fixedDim] = 1;
			int oppositeIndex = calcIndex(idx_n, allCellNums);

			while (i != NULL) {	// iterate over all particles in the cell
				Particle& p = *i->p;
				utils::Vector<double, 3>& x = p.getX();
				//ParticleList *cpy = pl;
				//pl = pl->next;
				if (fixedVal == 0) {
					//LOG4CXX_DEBUG(particlelog, "x before (fixedVal=0)  " <<allCells[calcIndex(idx, allCellNums)]->root->p->getX().toString());
					x[fixedDim] += radius*cellNums[fixedDim];
					//LOG4CXX_DEBUG(particlelog, "x after (fixedVal=0)  " <<allCells[calcIndex(idx, allCellNums)]->root->p->getX().toString());
					//cout<<"x value:"<<cells[calcIndex(idx, cellNums)].root->p->toString()<<endl;
				}
				else {
					//LOG4CXX_DEBUG(particlelog, "x before (fixedVal=cellNums)  " << p.getX().toString());
					x[fixedDim] -= radius*cellNums[fixedDim];
					//LOG4CXX_DEBUG(particlelog, "x after (fixedVal=cellNums)  " << p.getX().toString());
				}
				//LOG4CXX_DEBUG(particlelog, "opposite cell" << oppositeIndex);
				deleteList(pl);
				insertList(&allCells[oppositeIndex]->root, i);
				i = *pl;
			}
			//LOG4CXX_DEBUG(particlelog, "cell finished");
			//LOG4CXX_DEBUG(particlelog, "index:" << idx[0] << "," << idx[1] << "," << idx[2]);
			//LOG4CXX_DEBUG(particlelog, "index_n:" << idx_n[0] << "," << idx_n[1] << "," << idx_n[2]);

			// set own content to copy of opposite boundary cell's content
			allCells[index]->root = allCells[oppositeIndex]->root;

			/*ParticleList *pl2 = allCells[oppositeIndex]->root;
			while (pl2 != NULL) {
				Particle& p = *pl2->p;
				Particle *ghost = new Particle;
				utils::Vector<double, 3>& x = ghost->getX();
				x = p.getX();
				if (fixedVal == 0)
					x[fixedDim] -= radius*cellNums[fixedDim];
				else
					x[fixedDim] += radius*cellNums[fixedDim];
				ParticleList *pl_n = new ParticleList;
				pl_n->next = NULL;
				pl_n->p = ghost;
				insertList(&allCells[index]->root, pl_n);
				pl2 = pl2->next;
			}*/
		}
	//LOG4CXX_DEBUG(particlelog, "end periodic");
}

void ParticleContainerLC::emptyHaloSide(int fixedDim, int fixedVal) {
	int idx[DIM];
	idx[fixedDim] = fixedVal;
#if 1<DIM
	const int nextDim = (fixedDim+1) % DIM;
#endif
#if 3==DIM
	const int lastDim = (fixedDim+2) % DIM;
#endif
	// iterate over wall cells
#if 1<DIM
	for (idx[nextDim] = 0; idx[nextDim] < allCellNums[nextDim]; idx[nextDim]++)
#endif
#if 3==DIM
		for (idx[lastDim] = 0; idx[lastDim] < allCellNums[lastDim]; idx[lastDim]++)
#endif
		{
			allCells[calcIndex(idx, allCellNums)]->root = NULL;
		}
	LOG4CXX_DEBUG(particlelog, "end empty halo");
}
