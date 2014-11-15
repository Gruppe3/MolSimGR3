/*
 * LennardJonesTest.h
 *
 *  Created on: 03.11.2014
 *      Author: christiansturm2
 */

#ifndef SRC_TEST_LENNARDJONESTEST_H_
#define SRC_TEST_LENNARDJONESTEST_H_

#include <cppunit/TestCase.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include "../forces/LennardJones.h"

using namespace CppUnit;

/** Defines test suite for Lennard-Jones force calculation */
class LennardJonesTest : public TestCase {
public:
	/** constructor */
	LennardJonesTest();
	/** destructor */
	virtual ~LennardJonesTest();

	/** creating some Particles for test setup */
	void setUp();
	/** deletes particles */
	void tearDown();

	/** compares calculated force norm with manual calculation */
	void testValue();

	static Test *suite ()  {
		TestSuite *suiteOfTests = new TestSuite;
		suiteOfTests->addTest (new TestCaller<LennardJonesTest>("testValue", &LennardJonesTest::testValue));
		return suiteOfTests;
	}

private:
	Particle* p1;
	Particle* p2;
};

#endif /* SRC_TEST_LENNARDJONESTEST_H_ */
