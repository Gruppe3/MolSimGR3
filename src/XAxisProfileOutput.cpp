/*
 * XAxisProfileOutput.cpp
 *
 *  Created on: 19.01.2015
 *      Author: vmuser
 */

#include <XAxisProfileOutput.h>

#include "Logger.h"

const LoggerPtr iolog(Logger::getLogger("molsim.io"));
XAxisProfile::XAxisProfile() {

}

XAxisProfile::~XAxisProfile() {
	delete [] density;
	delete [] meanVelocity;
	delete [] particlesNum;
}

void XAxisProfile::setValues(int numBins_tag, double* domainSize_tag) {
	numBins = numBins_tag;
	meanVelocity = new double[numBins];
	density = new double[numBins];
	domainSize = domainSize_tag;
	binsLength = domainSize[0] / (double) numBins;
	particlesNum = new int[numBins];
	for (int i = 0; i < numBins; i++){
		particlesNum[i] = 0;
		density[i] = 0;
		meanVelocity[i] = 0;
	}
	fileOpened = false;
}

void XAxisProfile::openFile(int iteration) {
	fileOpened = true;
	stringstream strstr;
	strstr << "XAxisProfileOutput" << "_"
			<< (iteration < 10 ?
					"000" :
					(iteration < 100 ? "00" : (iteration < 1000 ? "0" : "")))
			<< iteration << ".csv";
	file.open(strstr.str().c_str());
	file
			<< "# in each line there are number of bins times density and velocity for each iteration \n";
	file << "# density     velocity at y component \n";
}

void XAxisProfile::iterateFunc(Particle& p) {
	if(&p!=NULL){
		int idx = p.getX()[0] / binsLength;
		//LOG4CXX_DEBUG(iolog, "index of particles for bins: "<<idx);
		particlesNum[idx]++;
		density[idx] +=  p.getM();
		meanVelocity[idx] += p.getV()[1];
	}
}
void XAxisProfile::writeInFile() {
	//LOG4CXX_DEBUG(iolog, "number of bins: "<<numBins);
	for (int i = 0; i < numBins; i++) { //prohibits division by 0
		if (particlesNum[i] == 0) {
			particlesNum[i]++;
		}
		density[i] = density[i]/(binsLength*domainSize[1]*domainSize[2]);
		meanVelocity[i] /= particlesNum[i];
		file << density[i] << ",";
		file << meanVelocity[i] << ",  ";
	}
	for (int i = 0; i < numBins; i++){
		particlesNum[i] = 0;
		density[i] = 0;
		meanVelocity[i] = 0;
	}
		file << "\n";
	}
void XAxisProfile::closeFile() {
	if (fileOpened){
		fileOpened = false;
		file.close();
	}
}
