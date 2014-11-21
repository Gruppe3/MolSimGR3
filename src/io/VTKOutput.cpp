/*
 * VTKOutput.cpp
 *
 *  Created on: 31.10.2014
 *      Author: christiansturm
 */

#include "VTKOutput.h"
#include "outputWriter/VTKWriter.h"

VTKOutput::~VTKOutput() {

}

void VTKOutput::setOutput(string fileName, int iteration, ParticleContainer* pc) {
	outputWriter::VTKWriter writer;
	writer.initializeOutput(pc->size());

	pc->resetIterator();
	while (pc->hasNext()) {
		writer.plotParticle(pc->next());
	}
	writer.writeFile(fileName, iteration);
}
