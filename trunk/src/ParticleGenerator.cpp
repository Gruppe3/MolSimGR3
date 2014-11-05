/*
 * ParticleGenerator.cpp
 *
 *  Created on: 03.11.2014
 *      Author: christiansturm
 */

#include "ParticleGenerator.h"
#include "MaxwellBoltzmannDistribution.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

ParticleGenerator::ParticleGenerator() {

}

void ParticleGenerator::getFileInput(char* fileName, ParticleContainer& pc) {
	double m = 1;
	int num_cuboids = 0;

	std::ifstream input_file(fileName);
	string tmp_string;

	if (input_file.is_open()) {
		getline(input_file, tmp_string);
		cout << "Read line: " << tmp_string << endl;

		while (tmp_string.size() == 0 || tmp_string[0] == '#') {
			getline(input_file, tmp_string);
			cout << "Read line: " << tmp_string << endl;
		}

		istringstream numstream(tmp_string);
		numstream >> num_cuboids;
		cout << "Reading " << num_cuboids << "." << endl;
		getline(input_file, tmp_string);
		cout << "Read line: " << tmp_string << endl;

		for (int i = 0; i < num_cuboids; i++) {
			istringstream datastream(tmp_string);

			for (int j = 0; j < 3; j++) {
				datastream >> X[j];
			}
			for (int j = 0; j < 3; j++) {
				datastream >> V[j];
			}

			for (int j = 0; j < 3; j++) {
				datastream >> num[j];
			}
			if (datastream.eof()) {
				cout << "Error reading file: eof reached unexpectedly reading from line " << i << endl;
				exit(-1);
			}

			datastream >> meanV;
			datastream >> M;
			datastream >> H;

			// create particles from cuboid data
			createParticles(pc);

			getline(input_file, tmp_string);
		}
	} else {
		std::cout << "Error: could not open file " << fileName << std::endl;
		exit(-1);
	}

}

ParticleGenerator::ParticleGenerator(utils::Vector<double, 3>& x, utils::Vector<int, 3>& n,
			utils::Vector<double, 3>& v, double h, double m, double meanv) {
	X = x;
	num = n;
	V = v;
	H = h;
	M = m;
	meanV = meanv;
}


ParticleGenerator::~ParticleGenerator() {

}

void ParticleGenerator::createParticles(ParticleContainer& pc) {
	for (int i = 0; i < num[0]; i++) {	// X dimension
		for (int j = 0; j < num[1]; j++) {	// Y dimension
			for (int k = 0; k < num[2]; k++) {	// Z dimension
				utils::Vector<double, 3> x;
				// calculate position
				x[0] = X[0] + i * H;
				x[1] = X[1] + j * H;
				x[2] = X[2] + k * H;

				Particle p(x, V, M, 0);
				MaxwellBoltzmannDistribution(p, meanV, 3);
				pc.add(p);
			}
		}
	}
}
