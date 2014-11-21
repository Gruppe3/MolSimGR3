/*
 * XYZOutput.h
 *
 *  Created on: 03.11.2014
 *      Author: christiansturm2
 */

#ifndef SRC_IO_XYZOUTPUT_H_
#define SRC_IO_XYZOUTPUT_H_

#include "OutputHandler.h"

/** Writes calculated data into an xyz file */
class XYZOutput : OutputHandler {
public:
	/** destructor */
	virtual ~XYZOutput();

	/** stores into xyz file; implementation of OutputHandler::setOutput */
	void setOutput(string fileName, int iteration, ParticleContainer* pc);
};

#endif /* SRC_IO_XYZOUTPUT_H_ */
