/*
 * ParticleContainerTest.cpp
 *
 *  Created on: 02.11.2014
 *      Author: christiansturm
 */

#include "ParticleContainerTest.h"

ParticleContainerTest::~ParticleContainerTest() {
	// TODO Auto-generated destructor stub
}

void ParticleContainerTest::setUp() {
	pc = new ParticleContainer();
	Particle p;
	Particle p2;
	Particle p3;
	pc->add(p);
	pc->add(p2);
	pc->add(p3);
}

void ParticleContainerTest::tearDown() {
	delete pc;
}

void ParticleContainerTest::testSize() {
	CPPUNIT_ASSERT(pc->size() == 3);
}

