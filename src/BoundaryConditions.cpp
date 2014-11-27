/*
 * BoundaryConditions.cpp
 *
 *  Created on: 27.11.2014
 *      Author: christiansturm
 */

#include "BoundaryConditions.h"

BoundaryConditions::~BoundaryConditions() {
	// TODO Auto-generated destructor stub
}

void BoundaryConditions::setBoundary(Side s, Boundary b) {
	domainBoundaries[s] = b;
}

BoundaryConditions::Boundary BoundaryConditions::getBoundary(Side s) {
	return domainBoundaries[s];
}
