/*
 * VTKOutput.h
 *
 *  Created on: 03.11.2014
 *      Author: christiansturm2
 */

#ifndef SRC_IO_VTKOUTPUT_H_
#define SRC_IO_VTKOUTPUT_H_

#include "OutputHandler.h"

class VTKOutput : OutputHandler {
public:
	virtual ~VTKOutput();

	/** stores into vtk file; implementation of OutputHanlder::setOutput */
	void setOutput(string fileName, int iteration, ParticleContainer& pc);
};

#endif /* SRC_IO_VTKOUTPUT_H_ */
