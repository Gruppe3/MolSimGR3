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
#include "Simulation.h"

/** Generates particle containers from cuboid data */
class ParticleGenerator : public InputHandler {
public:
	/** empty constructor */
	ParticleGenerator();
	/** reads multiple cuboids from file
	 * @param fileName file containing cuboid parameter lists
	 * @param pc ParticleContainer to store created particles */
	void getFileInput(char* fileName, ParticleContainer* pc, Simulation *sim);

	/** constructor for direct value input */
	ParticleGenerator(utils::Vector<double, 3>& X, utils::Vector<int, 3>& n,
			utils::Vector<double, 3>& v, double h, double m, double mean_v);

	/** destructor */
	virtual ~ParticleGenerator();

	/** creates particles due to cuboid parameters
	 * @param pc ParticleContainer storing created particles */
	void createParticles(ParticleContainer* pc);

	/** creates particles due to cuboid parameters
	 * @param x 3 dimensional location vector of the cuboid
	 * @param n 3 dimensional vector defining number of particles per dimension of the cuboid
	 * @param v 3 dimensional velocity vector of the cuboid
	 * @param h meshwidth/ distance between two particles
	 * @param m mass of each particle
	 * @param meanv mean value of Brownian Motion
	 * @param pc ParticleContainer storing created particles */
	void createCuboid(utils::Vector<double, 3>& x, utils::Vector<int, 3>& n,
			utils::Vector<double, 3>& v, double h, double m, double meanv, ParticleContainer* pc);

	/** creates particles due to sphere parameters
	 * @param x 3 dimensional location vector of the sphere's center
	 * @param n number of particles along the sphere's radius
	 * @param v 3 dimensional velocity vector of the sphere
	 * @param h meshwidth/ distance between two particles
	 * @param m mass of each particle
	 * @param meanv mean value of Brownian Motion
	 * @param pc ParticleContainer storing created particles */
	void createSphere(utils::Vector<double, 3>& x, int n,
			utils::Vector<double, 3>& v, double h, double m, double meanv, ParticleContainer* pc);

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
