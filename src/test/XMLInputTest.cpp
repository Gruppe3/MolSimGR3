/*
 * XMLInputTest.cpp
 *
 *  Created on: 21.11.2014
 *      Author: christiansturm
 */

#include "XMLInputTest.h"
#include "../ParticleGenerator.h"
#include "Logger.h"

const LoggerPtr iolog(log4cxx::Logger::getLogger("molsim.io"));

XMLInputTest::~XMLInputTest() {
	// TODO Auto-generated destructor stub
}

void XMLInputTest::setUp() {
	pc1 = new ParticleContainer;
	pc2 = new ParticleContainer;
}

void XMLInputTest::tearDown() {
	delete pc1;
	delete pc2;
}

void XMLInputTest::testParticleGen() {
	LOG4CXX_DEBUG(iolog, "running XMLInputTest::testParticleGen");
	XMLInput xi;
	xi.getFileInput("eingabe-two_bodies.xml", pc1, NULL);

	// manual data input
	ParticleGenerator pg;
	utils::Vector<double, 3> x, x2;
	utils::Vector<int, 3> n, n2;
	utils::Vector<double, 3> v, v2;
	x[0] = 20.0;
	x[1] = 20.0;
	x[2] = 0.0;

	n[0] = 100;
	n[1] = 20;
	n[2] = 1;

	v[0] = 0.0;
	v[1] = 0.0;
	v[2] = 0.0;
	pg.createCuboid(x, n, v, 1.1225, 1.0, 0.1, pc2);
	x2[0] = 70.0;
	x2[1] = 60.0;
	x2[2] = 0.0;

	n2[0] = 20;
	n2[1] = 20;
	n2[2] = 1;

	v2[0] = 0.0;
	v2[1] = -10.0;
	v2[2] = 0.0;
	pg.createCuboid(x2, n2, v2, 1.1225, 1.0, 0.1, pc2);

	CPPUNIT_ASSERT(pc1->size() == pc2->size());
	int i = 0;
	while (pc1->hasNext() && i < 20) {
		Particle p1 = pc1->next();
		Particle p2 = pc2->next();
		LOG4CXX_DEBUG(iolog, "i: " << i << " x1: " << p1.getX().toString() << " x2: " << p2.getX().toString());
		CPPUNIT_ASSERT(p1.getX() == p2.getX());
		i++;
	}
}
