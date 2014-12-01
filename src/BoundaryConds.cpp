/*
 * BoundaryConditions.cpp
 *
 *  Created on: 27.11.2014
 *      Author: christiansturm
 */

#include "BoundaryConds.h"

BoundaryConds::BoundaryConds() {
	for (int i = 0; i < 6; i++)
		domainBoundaries[i] = OUTFLOW;
}

BoundaryConds::~BoundaryConds() {
	// TODO Auto-generated destructor stub
}

void BoundaryConds::setBoundary(Side s, Boundary b) {
	domainBoundaries[s] = b;
}

BoundaryConds::Boundary BoundaryConds::getBoundary(Side s) {
	return domainBoundaries[s];
}
