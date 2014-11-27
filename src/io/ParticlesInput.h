/*
 * ParticlesInput.h
 *
 *  Created on: 31.10.2014
 *      Author: christiansturm
 */

#ifndef SRC_IO_PARTICLESINPUT_H_
#define SRC_IO_PARTICLESINPUT_H_

#include "InputHandler.h"

/** Reads particle data from an input file */
class ParticlesInput : public InputHandler {
public:
	virtual ~ParticlesInput();

	/** reads particles from file; implementation of InputHandler::getFileInput
	 * @param fileName File containing particle list
	 * @param pc ParticleContainer to store particles*/
	void getFileInput(char* fileName, ParticleContainer* pc, Simulation *sim);
};

#endif /* SRC_IO_PARTICLESINPUT_H_ */
