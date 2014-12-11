#ifndef SRC_PHASESPACEOUTPUT_H_
#define SRC_PHASESPACEOUTPUT_H_

#include "utils/Vector.h"
#include "ParticleContainerLC.h"
#include "Particle.h"
#include <iostream>
#include <fstream>


class PSO : public PCApply {
public:
	PSO(){
	}

	virtual ~PSO();

	void openFile();
	void closeFile();
	void iterateFunc(Particle& p);

private:
	ofstream outFile;
};


#endif /* SRC_PHASESPACEOUTPUT_H_ */