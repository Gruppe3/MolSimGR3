/*
 * ParticleGenerator.cpp
 *
 *  Created on: 03.11.2014
 *      Author: christiansturm
 */

#include "ParticleGenerator.h"
#include "MaxwellBoltzmannDistribution.h"
#include "Logger.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

const LoggerPtr particlelog(Logger::getLogger("molsim.particle"));
const LoggerPtr iolog(log4cxx::Logger::getLogger("molsim.io"));

ParticleGenerator::ParticleGenerator() {

}

void ParticleGenerator::getFileInput(char* fileName, ParticleContainer* pc, Simulation *sim) {
	double m = 1;
	int num_cuboids = 0;

	std::ifstream input_file(fileName);
	string tmp_string;

	if (input_file.is_open()) {
		getline(input_file, tmp_string);
		LOG4CXX_DEBUG(iolog, "Read line: " << tmp_string);

		while (tmp_string.size() == 0 || tmp_string[0] == '#') {
			getline(input_file, tmp_string);
			LOG4CXX_DEBUG(iolog, "Read line: " << tmp_string);
		}

		istringstream numstream(tmp_string);
		numstream >> num_cuboids;
		LOG4CXX_DEBUG(iolog, "Reading " << num_cuboids << ".");
		getline(input_file, tmp_string);
		LOG4CXX_DEBUG(iolog, "Read line: " << tmp_string);

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
				LOG4CXX_ERROR(iolog, "Error reading file: eof reached unexpectedly reading from line " << i);
				exit(-1);
			}

			datastream >> meanV;
			datastream >> M;
			datastream >> H;

			// create particles from cuboid data
			LOG4CXX_ERROR(particlelog, "Generating Cuboid...");
			createParticles(pc);

			getline(input_file, tmp_string);
		}
	} else {
		LOG4CXX_ERROR(iolog, "Error: could not open file " << fileName);
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

void ParticleGenerator::createParticles(ParticleContainer* pc) {
	int dim = num[2] <= 1 ? 2 : 3;

	for (int i = 0; i < num[0]; i++) {	// X dimension
		for (int j = 0; j < num[1]; j++) {	// Y dimension
			for (int k = 0; k < num[2]; k++) {	// Z dimension
				utils::Vector<double, 3> x;
				// calculate position
				x[0] = X[0] + i * H;
				x[1] = X[1] + j * H;
				x[2] = X[2] + k * H;
				Particle p(x, V, M, 0);
				MaxwellBoltzmannDistribution(p, meanV, dim );
				pc->add(p);
			}
		}
	}
}

void ParticleGenerator::createCuboid(utils::Vector<double, 3>& xn, utils::Vector<int, 3>& n,
		utils::Vector<double, 3>& v, double h, double m, double meanv, ParticleContainer* pc, Simulation * sim, int particleType) {

	int dim = n[2] <= 1 ? 2 : 3;
	Particle* cP;															//current Particle; just an aid that points to the Particle that the neighbour-assigning routine further below for the membrane is currently working on.
	//Particle* membraneBuffer [n[0]] [n[1]];										//matrix of particle pointers that the neighbour-assigning routine for the membrane works on.
	int length = n[0]*n[1]*n[2];
	int idx=0;
	//1 D array because of less pointers
	Particle **membraneBuffer1D;
	membraneBuffer1D = new Particle*[length];
	//membraneBuffer1D = (Particle**)malloc(length*sizeof(Particle*));
	for (int i = 0; i < n[0]; i++) {	// X dimension
		for (int j = 0; j < n[1]; j++) {	// Y dimension
			for (int k = 0; k < n[2]; k++) {	// Z dimension
				utils::Vector<double, 3> x;
				// calculate position
				x[0] = xn[0] + i * h;
				x[1] = xn[1] + j * h;
				x[2] = xn[2] + k * h;

				//Particle p = Particle(x, v, m, particleType);
				Particle* pPtr = new Particle(x, v, m, particleType);
				if (sim->meanVelocityTypeFlag == 1) {
					CalcT::initializeV(&meanv, sim->initTemp, pPtr->getM());
				}
				int type = pPtr->getType();
				MaxwellBoltzmannDistribution(*pPtr, meanv, dim, sim->typeflag, sim->states[type]);
				if (sim->membrane && k == 0) {
					//alocates memory for poiters to the neighbour cells and sets them to null
					pPtr->Neighbour = new Particle*[8];
					for (int d = 0; d < 8 ; d++){
						pPtr->Neighbour[d] = NULL;
					}
					//membraneBuffer [i] [j] = &p;	//fills up the membraneBuffer during particle generation, only if the Simulation is for a membrane of course.
					//LOG4CXX_DEBUG(iolog, "membraneBuffer[i][j]: "<<membraneBuffer[i][j]->toString()<<" i: "<<i<<" j: "<<j);
					membraneBuffer1D[idx] = pPtr;
					//LOG4CXX_DEBUG(iolog, "membraneBuffer[i]: "<<membraneBuffer1D[idx]->toString()<<" idx: "<<idx);
					idx++;
				}
				pc->add(*pPtr);
			}
		}
	}
	
	if (sim->membrane) {											//the neighbour-assigning routine for the membrane.
		for (int i = 0; i < n[0]; i++) {	// X dimension
			for (int j = 0; j < n[1]; j++) {	// Y dimension
				cP = membraneBuffer1D[i + j*n[0]];
				//LOG4CXX_DEBUG(iolog, "membraneBuffer1D[i,j]: "<<membraneBuffer1D[i*n[1]+j]->toString()<<" i: "<<i<<" j: "<<j);
				/*//these four assign the left, right, bottom and top neighbours to the current Particle cP in this order.
				if (i != 0) {cP->Neighbour[1] = membraneBuffer1D[(i - 1) * n[1] + j];}
				if (i != n[0]-1) {cP->Neighbour[5] = membraneBuffer1D [(i + 1) * n[1] + j];}
				if (j != 0) {cP->Neighbour[7] = membraneBuffer1D [i * n[1] + j - 1];}
				if (j != n[1]-1) {cP->Neighbour[3] = membraneBuffer1D [i * n[1] + j + 1];}

				//these four assign the diagonal neighbours: bottom left, top left, bottom right, top right. In this order.
				if (i != 0 && j != 0) {cP->Neighbour[0] = membraneBuffer1D [(i - 1)*n[1] + j - 1];}
				if (i != 0 && j != n[1] - 1) {cP->Neighbour[2] = membraneBuffer1D [(i - 1)*n[1] + j + 1];}
				if (i != n[0]-1 && j != 0) {cP->Neighbour[6] = membraneBuffer1D [(i + 1)*n[1] + j - 1];}
				if (i != n[0]-1 && j != n[1]-1) {cP->Neighbour[4] = membraneBuffer1D [(i + 1)*n[1] + j + 1];}
*/

				//these four assign the left, right, bottom and top neighbours to the current Particle cP in this order.
				std::list<Particle>::iterator it = pc->particles.begin();
				if (i != 0) {
					for (int pos = 0; pos < (i-1) + j * n[0]; pos++) it++;
					cP->Neighbour[1] = &(*it);
				}

				if (i != n[0]-1) {
					it = pc->particles.begin();
					for (int pos = 0; pos < (i+1) + j * n[0]; pos++) it++;
					cP->Neighbour[5] = &(*it);
				}

				if (j != 0) {
					it = pc->particles.begin();
					for (int pos = 0; pos < i + (j-1) * n[0]; pos++) it++;
					cP->Neighbour[7] = &(*it);
				}

				if (j != n[1]-1) {
					it = pc->particles.begin();
					for (int pos = 0; pos < i + (j+1) * n[0]; pos++) it++;
					cP->Neighbour[3] = &(*it);
				}

				//these four assign the diagonal neighbours: bottom left, top left, bottom right, top right. In this order.
				if (i != 0 && j != 0) {
					it = pc->particles.begin();
					for (int pos = 0; pos < (i-1) + (j-1) * n[0]; pos++) it++;
					cP->Neighbour[0] = &(*it);
				}

				if (i != 0 && j != n[1]-1) {
					it = pc->particles.begin();
					for (int pos = 0; pos < (i-1) + (j+1) * n[0]; pos++) it++;
					cP->Neighbour[2] = &(*it);
				}

				if (i != n[0]-1 && j != 0) {
					it = pc->particles.begin();
					for (int pos = 0; pos < (i+1) + (j-1) * n[0]; pos++) it++;
					cP->Neighbour[6] = &(*it);
				}

				if (i != n[0]-1 && j != n[1]-1) {
					it = pc->particles.begin();
					for (int pos = 0; pos < (i+1) + (j+1) * n[0]; pos++) it++;
					cP->Neighbour[4] = &(*it);
				}

				//pc->add(*cP);
				/*cP = membraneBuffer [i] [j];
				LOG4CXX_DEBUG(iolog, "membraneBuffer[i][j]: "<<membraneBuffer[i][j]->toString()<<" i: "<<i<<" j: "<<j);
				if (i != 0) {(*cP).Neighbour[1] = membraneBuffer [i - 1] [j];}
				//these four assign the left, right, bottom and top neighbours to the current Particle cP in this order.
				if (i != n[0] - 1) {(*cP).Neighbour[5] = membraneBuffer [i + 1] [j];}
				if (j != 0) {(*cP).Neighbour[7] = membraneBuffer [i] [j - 1];}
				if (j != n[1] - 1) {(*cP).Neighbour[3] = membraneBuffer [i] [j + 1];}

				if (i != 0 && j != 0) {(*cP).Neighbour[0] = membraneBuffer [i - 1] [j - 1];}				//these four assign the diagonal neighbours: bottom left, top left, bottom right, top right. In this order.
				if (i != 0 && j != n[1] - 1) {(*cP).Neighbour[2] = membraneBuffer [i - 1] [j + 1];}
				if (i != n[0] - 1 && j != 0) {(*cP).Neighbour[6] = membraneBuffer [i + 1] [j - 1];}
				if (i != n[0] - 1 && j != n[1] - 1) {(*cP).Neighbour[4] = membraneBuffer [i + 1] [j + 1];}*/


				/*for (int d=0;d<8;d++){
					if(cP->Neighbour[d]!=NULL)
					LOG4CXX_DEBUG(iolog, "cp.Neighbour: "<<cP->Neighbour[d]->toString()<<" idx: "<<d);
				}
				LOG4CXX_DEBUG(iolog, "cp: "<<cP->toString()<<" idx: "<<i*n[1]+j);
				}*/
			}
		}
		//LOG4CXX_INFO(iolog, "neighbours added ");
	}
}

void ParticleGenerator::createSphere(utils::Vector<double, 3>& x, int n,
			utils::Vector<double, 3>& v, double h, double m, double meanv, int brownDim, ParticleContainer* pc, Simulation * sim, int particleType) {

	for (int i = -n; i <= n; i++) {	// X dimension
		for (int j = -n; j <= n; j++) {	// Y dimension
			for (int k = -n; k <= n; k++) {	// Z dimension
				utils::Vector<double, 3> xn;

				if (sqrt(i * i + j * j + k * k) <= n) {								//checks if proposed particle position is within the radius
					// calculate position
					xn[0] = x[0] + i * h;
					xn[1] = x[1] + j * h;
					xn[2] = x[2] + k * h;

					Particle p(xn, v, m, 0);
					if(sim->meanVelocityTypeFlag==1){
						CalcT::initializeV(&meanv,sim->initTemp,p.getM());
					}
					//cout<<"meanV     "<<meanv<<endl;
					MaxwellBoltzmannDistribution(p, meanv, brownDim );
					pc->add(p);
				}
			}
		}
	}
}
