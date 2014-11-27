/*
 * VTKOutput.cpp
 *
 *  Created on: 31.10.2014
 *      Author: christiansturm
 */

#include "VTKOutput.h"
#include "outputWriter/VTKWriter.h"
#include "Logger.h"

const LoggerPtr iolog(log4cxx::Logger::getLogger("molsim.io"));

VTKOutput::~VTKOutput() {

}

void VTKOutput::setOutput(string fileName, int iteration, ParticleContainer* pc) {
	outputWriter::VTKWriter writer;
	writer.initializeOutput(pc->size());

	pc->resetIterator();
	//LOG4CXX_DEBUG(iolog, "output: after reset" << pc->size());
	while (pc->hasNext()) {
		writer.plotParticle(pc->next());
	}
	writer.writeFile(fileName, iteration);
}
