/*
 * ParticleGenerator.h
 *
 *  Created on: 31.10.2014
 *      Author: christiansturm
 */

#ifndef SRC_PARTICLEGENERATOR_H_
#define SRC_PARTICLEGENERATOR_H_

#include "utils/Vector.h"
#include "Particle.h"
#include "ParticleContainer.h"
#include "io/InputHandler.h"

/** Generates particle containers from cuboid data */
class ParticleGenerator : InputHandler {
public:
	/** empty constructor */
	ParticleGenerator();
	/** reads multiple cuboids from file
	 * @param fileName file containing cuboid parameter lists
	 * @param pc ParticleContainer to store created particles */
	void getFileInput(char* fileName, ParticleContainer& pc);

	/** constructor for direct value input */
	ParticleGenerator(utils::Vector<double, 3>& X, utils::Vector<int, 3>& n,
			utils::Vector<double, 3>& v, double h, double m, double mean_v);

	/** destructor */
	virtual ~ParticleGenerator();

	/** creates particles due to cuboid parameters
	 * @param pc ParticleContainer storing created particles */
	void createParticles(ParticleContainer& pc);

private:
	/** 3 dimensional position of the cuboid (lower left front-side corner) */
	utils::Vector<double, 3> X;

	/** number of particles per dimention (X, Y, Z) */
	utils::Vector<int, 3> num;

	/** distance of particles in the cuboid */
	double H;

	/** mass of each particle in the cuboid */
	double M;

	/** initial velocity of particles in the cuboid */
	utils::Vector<double, 3> V;

	/** mean velocity of Brownian Motion */
	double meanV;
};

#endif /* SRC_PARTICLEGENERATOR_H_ */
