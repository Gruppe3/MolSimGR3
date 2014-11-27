/*
 * InputHandler.h
 *
 *  Created on: 31.10.2014
 *      Author: christiansturm
 */

#ifndef SRC_IO_INPUTHANDLER_H_
#define SRC_IO_INPUTHANDLER_H_

#include "../ParticleContainer.h"
#include "../Simulation.h"

/** An abstract class every input type (e.g.ParticlesInput) has to implement */
class InputHandler {
public:
	/** destructor */
	virtual ~InputHandler();

	/** abstract input method */
	virtual void getFileInput(char* fileName, ParticleContainer* pc, Simulation *sim) = 0;
};

#endif /* SRC_IO_INPUTHANDLER_H_ */
