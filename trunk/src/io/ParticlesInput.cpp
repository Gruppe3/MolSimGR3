/*
 * ParticlesInput.cpp
 *
 *  Created on: 31.10.2014
 *      Author: christiansturm
 */

#include "ParticlesInput.h"
#include "../FileReader.h"

ParticlesInput::~ParticlesInput() {

}

void ParticlesInput::getFileInput(char* fileName, ParticleContainer& pc) {
	FileReader fileReader;
	fileReader.readFile(pc, fileName);
}
