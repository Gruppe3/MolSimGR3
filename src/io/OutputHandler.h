/*
 * OutputHandler.h
 *
 *  Created on: 31.10.2014
 *      Author: christiansturm
 */

#ifndef SRC_IO_OUTPUTHANDLER_H_
#define SRC_IO_OUTPUTHANDLER_H_

#include "../ParticleContainer.h"

/** An abstract class every output type (e.g. VTKOutput)  has to implement */
class OutputHandler {
public:
	virtual ~OutputHandler();

	/** abstract output method
	 * @param fileName Storage file
	 * @param iteration Number of current time step
	 * @param pc ParticleContainer */
	virtual void setOutput(string fileName, int iteration, ParticleContainer& pc) = 0;
};

#endif /* SRC_IO_OUTPUTHANDLER_H_ */
