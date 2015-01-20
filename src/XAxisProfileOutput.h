/*
 * XAxisProfileOutput.h
 *
 *  Created on: 19.01.2015
 *      Author: vmuser
 */

#ifndef SRC_XAXISPROFILEOUTPUT_H_
#define SRC_XAXISPROFILEOUTPUT_H_

#include "utils/Vector.h"
#include "ParticleContainerLC.h"
#include "ParticleContainer.h"
#include "Particle.h"
#include <iostream>
#include <fstream>

class XAxisProfile: public PCApply {
public:
	XAxisProfile();
	virtual ~XAxisProfile();
	void setValues(int numBins_tag, double* domainSize_tag);
	void openFile(int iteration);
	void closeFile();
	void writeInFile();
	void iterateFunc(Particle& p);
private:
	ofstream file;
	/** mean velocity at Y component of the particles in parallelepiped defined by binsLength */
	double *meanVelocity;
	/** density of the particles in parallelepiped defined by binsLength */
	double *density;
	/** length along the x axis of the N bins */
	double binsLength;
	/** size of the domain, needed for calculation of density */
	double *domainSize;
	/** number of particles in each bin */
	int *particlesNum;
	/** number of bins */
	int numBins;
	/** if set there is an opened file */
	bool fileOpened;
};

#endif /* SRC_XAXISPROFILEOUTPUT_H_ */
