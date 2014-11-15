/*
 * ParticleGeneratorTest.h
 *
 *  Created on: 03.11.2014
 *      Author: christiansturm
 */

#ifndef SRC_TEST_PARTICLEGENERATORTEST_H_
#define SRC_TEST_PARTICLEGENERATORTEST_H_

#include <cppunit/TestCase.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <log4cxx/logger.h>

#include "../ParticleGenerator.h"
#include "../ParticleContainer.h"

using namespace CppUnit;
using namespace log4cxx;

/** defines test suite for ParticleGenerator */
class ParticleGeneratorTest : public TestCase {
public:
	ParticleGeneratorTest();
	virtual ~ParticleGeneratorTest();

	void setUp();
	void tearDown();

	void testSize();
	void testH();
	void testV();

	static Test *suite ()  {
		TestSuite *suiteOfTests = new TestSuite;
		suiteOfTests->addTest(new TestCaller<ParticleGeneratorTest>("testSize", &ParticleGeneratorTest::testSize));
		suiteOfTests->addTest(new TestCaller<ParticleGeneratorTest>("testH", &ParticleGeneratorTest::testH));
		suiteOfTests->addTest(new TestCaller<ParticleGeneratorTest>("testV", &ParticleGeneratorTest::testV));
		return suiteOfTests;
	}

private:
	ParticleContainer* pc;
	ParticleGenerator* pg;

	utils::Vector<double, 3> x;
	utils::Vector<int, 3> n;
	utils::Vector<double, 3> v;
	double h;
};

#endif /* SRC_TEST_PARTICLEGENERATORTEST_H_ */
