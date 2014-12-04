/*
 * ParticleContainerLCTest.h
 *
 *  Created on: 01.12.2014
 *      Author: christiansturm
 */

#ifndef SRC_TEST_PARTICLECONTAINERLCTEST_H_
#define SRC_TEST_PARTICLECONTAINERLCTEST_H_

#include <cppunit/TestCase.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include "../ParticleContainerLC.h"
#include "../Simulation.h"

using namespace CppUnit;

/** Defines test suite for ParticleContainerLC */
class ParticleContainerLCTest : public TestCase {
public:
	virtual ~ParticleContainerLCTest();

	void setUp();
	void tearDown();

	void testSize();

	/**tests if add() sets the particle in the correct cell*/
	void testAdd();

	/**tests if move() sets the particle in the correct cell*/
	void testMove();

	/** tests if moving particle stays still in the domain after multiple iterations */
	void testReflection();

	static Test *suite ()  {
		TestSuite *suiteOfTests = new TestSuite;
		suiteOfTests->addTest (new TestCaller<ParticleContainerLCTest>("testSize", &ParticleContainerLCTest::testSize));
		suiteOfTests->addTest (new TestCaller<ParticleContainerLCTest>("testAdd", &ParticleContainerLCTest::testAdd));
		suiteOfTests->addTest (new TestCaller<ParticleContainerLCTest>("testMove", &ParticleContainerLCTest::testMove));
		suiteOfTests->addTest (new TestCaller<ParticleContainerLCTest>("testReflection", &ParticleContainerLCTest::testReflection));
		return suiteOfTests;
	}

private:
	ParticleContainerLC* pc;
	Simulation *sim;
	Particle *p;
	Particle *p_;
};

#endif /* SRC_TEST_PARTICLECONTAINERLCTEST_H_ */
