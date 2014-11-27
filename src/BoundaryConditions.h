/*
 * BoundaryConditions.h
 *
 *  Created on: 27.11.2014
 *      Author: christiansturm
 */

#ifndef SRC_BOUNDARYCONDITIONS_H_
#define SRC_BOUNDARYCONDITIONS_H_

class BoundaryConditions {
public:
	/** types of boundary conditions */
	enum Boundary {OUTFLOW, REFLECTING};

	/** stores sides of the 3d domain;
	 * Attention: Don't change the order! */
	enum Side {LEFT, RIGHT, BOTTOM, TOP, FRONT, BACK};

	virtual ~BoundaryConditions();
	/** sets boundary b for domain side s */
	void setBoundary(Side s, Boundary b);

	/** returns boundary for domain side s */
	Boundary getBoundary(Side s);

private:
	/** stores boundary conditions in this order: left, right, bottom, top, front, back */
	Boundary domainBoundaries[6];
};

#endif /* SRC_BOUNDARYCONDITIONS_H_ */
