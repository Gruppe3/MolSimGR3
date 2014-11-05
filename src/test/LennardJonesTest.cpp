/*
 * LennardJonesTest.cpp
 *
 *  Created on: 03.11.2014
 *      Author: christiansturm2
 */

#include "LennardJonesTest.h"
#include "../utils/Vector.h"

LennardJonesTest::LennardJonesTest() {
	setUp();
}

void LennardJonesTest::setUp() {
	double x[3] = {0,0,0};
	double x_[3] = {0,0,0};
	utils::Vector<double, 3> x1 = x;
	utils::Vector<double, 3> x2 = x_;
	utils::Vector<double, 3> v = x;
	p1 = new Particle(x1, v, 1, 0);
	p2 = new Particle(x2, v, 1, 0);
}

LennardJonesTest::~LennardJonesTest() {

}

void LennardJonesTest::tearDown() {
	delete p1;
	delete p2;
}

void LennardJonesTest::testValue() {
	double f_man_norm = sqrt(2*5.625*5.625); // sigma = 1; epsilon = 5
	LennardJones lj;
	lj.calc(*p1, *p2);
	utils::Vector<double, 3>& f = p1->getF();
	CPPUNIT_ASSERT(abs(f.L2Norm() - f_man_norm) < 0.0005);
}
