/*
 * Simulation.h
 *
 *  Created on: 27.11.2014
 *      Author: christiansturm
 */

#ifndef SRC_SIMULATION_H_
#define SRC_SIMULATION_H_

#include <cstring>
#include <cstdlib>
#include <iostream>

#include "BoundaryConds.h"

using namespace std;

class Simulation {
public:
	Simulation() {
		start_time = 0;
		end_time = 1000;
		delta_t = 0.014;
		out_name = string("MD_vtk");
		writeFreq = 10;

		meshWidth = 1.1225;
		thermostatStart = 100;
		meanVelocityTypeFlag = 1;
		initTemp = 5.0;
		//diffTargetTempFlag=true;
		targetTemp = 500.0;
		tempDiff = 2;
		tempFreq = 1;

		sigmas = new double[1];
		sigmas[0] = 1.0;
		epsilons = new double[1];
		epsilons[0] = 5.0;
		cutoff = 2.5 * sigmas[0];
		//sigma21=sigma12;
		gravity = -9.807;
		//epsilon21=epsilon12;
		//boltzmannConst=1.3806488E-23;
		boltzmannConst = 1.0;
		boundaries = new BoundaryConds;
	}

	/** destructor */
	virtual ~Simulation() {
		delete [] sigmas;
		delete [] epsilons;
	};

	/** start time of the simulation */
	double start_time;

	/** end time of the simulation */
	double end_time;

	/** size of the time step */
	double delta_t;

	/** common prefix for output files */
	string out_name;

	/** number of iterations before next file will be written */
	int writeFreq;

	/** domain size for LC algo */
	double domainSize[3];

	/** cut off radius for LC algo */
	double cutoff;

	/** distance between particles (2^(1/6)*sigma) */
	double meshWidth;

	/** time step to start first thermostat application */
	int thermostatStart;

	/** initial temperature of the system */
	double initTemp;

	double boltzmannConst;

	//bool diffTargetTempFlag;

	/** desired temperature */
	double targetTemp;

	/** step size in which temperature should be changed */
	double tempDiff;

	/** type a + type b = index for sigma between types a and b */
	double *sigmas;

	/** type a + type b = index for epsilon between types a and b */
	double *epsilons;

	/** number of particle types */
	int typesNum;

	/**1 - initialize temperature for thermostat problem, 0 - initialize temperature only with Maxwell-Boltzmann*/
	int meanVelocityTypeFlag;

	/** time steps between thermostat applications */
	int tempFreq;

	/** gravitation force,that is applied to particles additionally*/
	double gravity;

	/** stores boundary conditions for LC algo */
	BoundaryConds *boundaries;
};



#endif /* SRC_SIMULATION_H_ */
