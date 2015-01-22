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

		typesNum=1;
		sigmas = new double[1];
		sigmas[0] = 1.0;
		epsilons = new double[1];
		epsilons[0] = 5.0;
		states = new string[1];
		states[0] = "none";
		typeflag = false;

		numOfBins = 1;
		cutoff = 2.5 * sigmas[0];
		//sigma21=sigma12;
		gravity = -9.807;
		forceType = string("LennardJones");
		rl = 1.9;
		//epsilon21=epsilon12;
		//boltzmannConst=1.3806488E-23;
		boltzmannConst = 1.0;
		boundaries = new BoundaryConds;
		membrane = false;
		stiffnessConstant = 300.0;
		forceUpwards = 0.8;
		harmonicR0 = 2.2;
		gravityUpwardsEnd = 15000;
		pulledUpParticlesLength = 8;
		pulledUpParticles = new int [pulledUpParticlesLength];
		pulledUpParticles[0] = 17; pulledUpParticles[1] = 24; pulledUpParticles[2] = 17; pulledUpParticles[3] = 25;
		pulledUpParticles[4] = 18; pulledUpParticles[5] = 24; pulledUpParticles[6] = 18; pulledUpParticles[7] = 25;
		cellsSize[0] = 0; cellsSize[1] = 0; cellsSize[2] = 0;
		flowKineticEnergy = 0;
		numParticles = 0;
		writeFreqFlowSim = 10000;
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

	/** state of matter for the particles according to their type */
	string *states;

	/** typeflag is set if state of matter has to be taken into account */
	bool typeflag;

	/** x axis is subdivided in N equal bins that content density- and velocity-profile of its particles */
	int numOfBins;

	/**1 - initialize temperature for thermostat problem, 0 - initialize temperature only with Maxwell-Boltzmann*/
	int meanVelocityTypeFlag;

	/** time steps between thermostat applications */
	int tempFreq;

	/** gravitation force, that is applied to particles additionally*/
	double gravity;

	/** defines force type to use */
	string forceType;

	/** for smoothed LJ force calculation */
	double rl;

	/** stores boundary conditions for LC algo */
	BoundaryConds *boundaries;

	/** Indicates whether the simulation uses neighbouring particles,  harmonic potential, and truncated LJ-potential as in the task "Simulation of a membrane".*/
	bool membrane;

	/** constant needed for the calculation of the harmonic potential */
	double  stiffnessConstant;

	/** gravitation force,that is applied to some particles upwards at z axis if membrane is set to true */
	double forceUpwards;

	/** defines till which iteration gravityUpwards to be called */
	int gravityUpwardsEnd;

	/** defines indices of the particles that will be pulled up with gravityUpwards*/
	int *pulledUpParticles;

	/** defines the length of the array i.e. number of particles to be pulled up */
	int pulledUpParticlesLength;

	/** the average bond length of a molecule pair, needed for the calculation of the harmonic potential */
	double harmonicR0;

	/** defines size of the cells in each dimention, used in EarthGravitation to define indices of the cells
	 * and check if particles should be pulled up*/
	double cellsSize[3];

	/** average kinetic energy at Y coordinate */
	double flowKineticEnergy;

	/** if set, mean velocity has to be divided by numParticles */
	bool meanVelocityCalculated;

	/** current number of particles */
	int numParticles;

	/** number of iterations before file is written (for density and velocity profile along x axis for flow simulation) */
	int writeFreqFlowSim;
};



#endif /* SRC_SIMULATION_H_ */
