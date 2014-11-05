/*
 * Runner.cpp
 *
 *  Created on: 02.11.2014
 *      Author: christiansturm
 */

#include <iostream>

#include "cppunit/TestSuite.h"
#include <cppunit/ui/text/TestRunner.h>

#include "ParticleContainerTest.h"

using namespace CppUnit;

/** run given test
 * @param tc test case meant to be run */
int run1 (Test* t) {
	TextUi::TestRunner runner;

	cout << "Creating Test Suites:" << endl;
	runner.addTest(t);
	cout<< "Running the unit tests."<<endl;
	runner.run();

	return 0;
}
