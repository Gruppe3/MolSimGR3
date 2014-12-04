/*
 * ParticleContainerLCTest.cpp
 *
 *  Created on: 01.12.2014
 *      Author: christiansturm
 */

#include "ParticleContainerLCTest.h"
#include "../DynamicsCalc.h"
#include "../forces/LennardJones.h"
#include "../Logger.h"

const LoggerPtr testlog(log4cxx::Logger::getLogger("molsim.test"));

ParticleContainerLCTest::~ParticleContainerLCTest() {
	// TODO Auto-generated destructor stub
}

void ParticleContainerLCTest::setUp() {

	sim = new Simulation;
	sim->cutoff = 3.0;
	sim->delta_t = 0.00005;
	sim->meshWidth = 1.1225;
	sim->boundaries->setBoundary(BoundaryConds::LEFT, BoundaryConds::REFLECTING);
	sim->boundaries->setBoundary(BoundaryConds::RIGHT, BoundaryConds::REFLECTING);
	sim->domainSize[0] = 6.0;
	sim->domainSize[1] = 6.0;
	sim->domainSize[2] = 3.0;

	ParticleContainer pc1;
	pc = new ParticleContainerLC(&pc1, sim);
	utils::Vector<double, 3> x(1.1);
	utils::Vector<double, 3> x_(5.1);
	x_[2]=0;
	utils::Vector<double, 3> v(0.0);
	v[0] = 10;
	p = new Particle(x, v, 1.0, 0);
	p_ = new Particle(x_, v, 1.0, 0);
	LOG4CXX_INFO(testlog, "p:" << p->toString());
	LOG4CXX_INFO(testlog, "p_:" << p_->toString());

	pc->add(*p);
	pc->add(*p_);

}

void ParticleContainerLCTest::tearDown() {
	delete pc;
	delete sim;
	delete p;
	delete p_;
}
void ParticleContainerLCTest::testSize() {
	CPPUNIT_ASSERT(pc->size() == 2);
}

void ParticleContainerLCTest::testAdd(){
	pc->resetIterator();
	if(pc[0].hasNextInCell()){
	Particle& tmp=pc[0].nextInCell();
	CPPUNIT_ASSERT(*p==tmp);
} else
	CPPUNIT_ASSERT(false);
}

void ParticleContainerLCTest::testReflection() {
	LOG4CXX_INFO(testlog, "testing reflection...");
	LOG4CXX_INFO(testlog, "container size start:" << pc->size());
	CPPUNIT_ASSERT(pc->size() == 2);

	ForceHandler* forceType = new LennardJonesLC();
	CalcX *xcalc = new CalcX(sim);
	CalcV *vcalc = new CalcV(sim);

	pc->iteratePair(forceType);
	pc->applyBoundaryConds(BoundaryConds::REFLECTING, forceType);

	for (int i = 0; i < 10000; i++) {
		pc->iterate(xcalc);
		pc->moveParticles();

		// copies F to oldF of particles and sets F to 0
		pc->iterate(forceType);
		// calculate new f
		pc->iteratePair(forceType);
		// add reflecting force to boundary particles according to sim->domainBoundaries[]
		pc->applyBoundaryConds(BoundaryConds::REFLECTING, forceType);

		// calculate new v
		pc->iterate(vcalc);
		pc->emptyHalo();

	}

	LOG4CXX_INFO(testlog, "container size end:" << pc->size());
	CPPUNIT_ASSERT(pc->size() == 2);

	pc->resetIterator();
	LOG4CXX_INFO(testlog, "p: " << pc->next().toString());
}



void ParticleContainerLCTest::testMove(){
	pc->resetIterator();
	//change coordinates of the second particle p_ 
	utils::Vector<double, 3>& x=p_->getX();
	x=2;
	//second particle should be moved in the first cell
	pc->moveParticles();
	pc->resetIterator();
		if(pc[0].hasNextInCell()){
		Particle& tmp=pc[0].nextInCell();
		CPPUNIT_ASSERT(*p_==tmp);
	} else
		CPPUNIT_ASSERT(false);
}
