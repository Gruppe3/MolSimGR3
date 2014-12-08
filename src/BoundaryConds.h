/*
 * BoundaryConditions.h
 *
 *  Created on: 27.11.2014
 *      Author: christiansturm
 */

#ifndef SRC_BOUNDARYCONDS_H_
#define SRC_BOUNDARYCONDS_H_

/** defines boundary conditions for all six sides of a 3D domain used in the LC algo */
class BoundaryConds {
public:
	/** types of boundary conditions */
	enum Boundary {OUTFLOW, REFLECTING, PERIODIC};

	/** stores sides of the 3d domain;
	 * Attention: Don't change the order! */
	enum Side {LEFT, RIGHT, BOTTOM, TOP, FRONT, BACK};

	/** sets initially all domainBoundaries to OUTFLOW */
	BoundaryConds();
	virtual ~BoundaryConds();
	/** sets boundary b for domain side s */
	void setBoundary(Side s, Boundary b);

	/** returns boundary for domain side s */
	Boundary getBoundary(Side s);

private:
	/** stores boundary conditions in this order: left, right, bottom, top, front, back;
	 * default value is OUTFLOW */
	Boundary domainBoundaries[6];
};

#endif /* SRC_BOUNDARYCONDS_H_ */
