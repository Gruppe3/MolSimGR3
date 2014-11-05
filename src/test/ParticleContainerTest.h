/*
 * ParticleContainerTest.h
 *
 *  Created on: 02.11.2014
 *      Author: christiansturm
 */

#ifndef TEST_PARTICLECONTAINERTEST_H_
#define TEST_PARTICLECONTAINERTEST_H_

#include <cppunit/TestCase.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include "../ParticleContainer.h"

using namespace CppUnit;

class ParticleContainerTest : public TestCase {
public:
	virtual ~ParticleContainerTest();

	void setUp();
	void tearDown();

	void testSize();

	static Test *suite ()  {
		TestSuite *suiteOfTests = new TestSuite;
	    suiteOfTests->addTest (new TestCaller<ParticleContainerTest>("testSize", &ParticleContainerTest::testSize));
	    return suiteOfTests;
	}

private:
	ParticleContainer* pc;
};

#endif
