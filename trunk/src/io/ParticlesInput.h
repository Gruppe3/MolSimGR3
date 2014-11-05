/*
 * ParticlesInput.h
 *
 *  Created on: 31.10.2014
 *      Author: christiansturm
 */

#ifndef SRC_IO_PARTICLESINPUT_H_
#define SRC_IO_PARTICLESINPUT_H_

#include "InputHandler.h"

class ParticlesInput : InputHandler {
public:
	virtual ~ParticlesInput();

	/** reads particles from file; implementation of InputHandler::getFileInput
	 * @param fileName File containing particle list
	 * @param pc ParticleContainer to store particles*/
	void getFileInput(char* fileName, ParticleContainer& pc);
};

#endif /* SRC_IO_PARTICLESINPUT_H_ */