
#include "ThermostatTest.h"
#include "../DynamicsCalc.h"
#include "../forces/LennardJones.h"
#include "../Logger.h"

const LoggerPtr testlog(log4cxx::Logger::getLogger("molsim.test"));

ThermostatTest::~ThermostatTest() {
	// TODO Auto-generated destructor stub
}

void ThermostatTest::setUp() {

	sim = new Simulation;
	sim->cutoff = 3.0;
	sim->delta_t = 0.00005;
	sim->meshWidth = 1.1225;
	sim->boundaries->setBoundary(BoundaryConds::LEFT, BoundaryConds::REFLECTING);
	sim->boundaries->setBoundary(BoundaryConds::RIGHT, BoundaryConds::REFLECTING);
	sim->domainSize[0] = 6.0;
	sim->domainSize[1] = 6.0;
	sim->domainSize[2] = 3.0;
	sim->initTemp = 10.0;


	ParticleContainer pc1;
	pc = new ParticleContainerLC(&pc1, sim);
	utils::Vector<double, 3> x(1.1);
	utils::Vector<double, 3> v(0.0);

	p = new Particle(x, v, 1.0, 0);
	LOG4CXX_INFO(testlog, "p:" << p->toString());

	pc->add(*p);

}

void ThermostatTest::tearDown() {
	delete pc;
	delete sim;
	delete p;
}

void ThermostatTest::testT() {
	CalcV *vcalc = new CalcV(sim);
	CalcT *tcalc = new CalcT(sim);

	pc->iterate(tcalc);

	Beta = tcalc->calcBeta(10.0, 1);

	LOG4CXX_INFO(testlog, "Beta: " << Beta);
	CPPUNIT_ASSERT(Beta == 1.0);
}

