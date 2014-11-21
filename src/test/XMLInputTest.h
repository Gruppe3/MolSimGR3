/*
 * XMLInputTest.h
 *
 *  Created on: 21.11.2014
 *      Author: christiansturm
 */

#ifndef SRC_TEST_XMLINPUTTEST_H_
#define SRC_TEST_XMLINPUTTEST_H_

#include <cppunit/TestCase.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <log4cxx/logger.h>
#include "../io/XMLInput.h"

using namespace CppUnit;
using namespace log4cxx;

/** defines test suite for ParticleGenerator */
class XMLInputTest : public TestCase {
public:
	virtual ~XMLInputTest();

	void setUp();
	void tearDown();

	void testParticleGen();

	static Test *suite ()  {
		TestSuite *suiteOfTests = new TestSuite;
		suiteOfTests->addTest(new TestCaller<XMLInputTest>("testSize", &XMLInputTest::testParticleGen));
		return suiteOfTests;
	}

private:
	ParticleContainer* pc1;
	ParticleContainer* pc2;
};

#endif /* SRC_TEST_XMLINPUTTEST_H_ */
