/*
 * ParticleGeneratorTest.cpp
 *
 *  Created on: 03.11.2014
 *      Author: christiansturm
 */

#include "ParticleGeneratorTest.h"
#include <iostream>

ParticleGeneratorTest::ParticleGeneratorTest() {


}

ParticleGeneratorTest::~ParticleGeneratorTest() {

}

void ParticleGeneratorTest::setUp() {
	pc = new ParticleContainer();
	x = utils::Vector<double, 3>(0.0);
	n = utils::Vector<int, 3>(2);
	v = utils::Vector<double, 3>(0.0);
	h = 1.0;
	pg = new ParticleGenerator(x, n, v, h, 1.0, 0.1);
	pg->createParticles(*pc);
}

void ParticleGeneratorTest::tearDown() {
	delete pc;
	delete pg;
}

void ParticleGeneratorTest::testSize() {
	LOG4CXX_DEBUG(Logger::getLogger("MolSim.test"), "-testSize");
	CPPUNIT_ASSERT(pc->size() == n[0]*n[1]*n[2]);
}

void ParticleGeneratorTest::testH() {
	LOG4CXX_DEBUG(Logger::getLogger("MolSim.test"), "-testH");
	Particle p1 = pc->next();
	Particle p2 = pc->next();
	utils::Vector<double, 3> h_vec;
	h_vec[0] = 0.0;
	h_vec[1] = 0.0;
	h_vec[2] = h;
	utils::Vector<double, 3> x1 = p1.getX();
	utils::Vector<double, 3> x2 = p2.getX();
	LOG4CXX_DEBUG(Logger::getLogger("MolSim.test"), "x1:" << x1.toString() << "; x2:" << x2.toString());
	CPPUNIT_ASSERT(x1 == (x2-h_vec));

	p2 = pc->next();
	h_vec[0] = 0.0;
	h_vec[1] = h;
	h_vec[2] = 0.0;
	x2 = p2.getX();
	CPPUNIT_ASSERT(x1 == (x2-h_vec));
}

void ParticleGeneratorTest::testV() {
	LOG4CXX_DEBUG(Logger::getLogger("MolSim.test"), "-testV");
	utils::Vector<double, 3> nul(0.0);
	while (pc->hasNext()) {
		Particle p = pc->next();
		CPPUNIT_ASSERT(!p.getV().equals(nul));
	}
}
