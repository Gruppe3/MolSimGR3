/*
 * XYZOutput.cpp
 *
 *  Created on: 31.10.2014
 *      Author: christiansturm
 */

#include "XYZOutput.h"
#include "outputWriter/XYZWriter.h"

XYZOutput::~XYZOutput() {

}

void XYZOutput::setOutput(string fileName, int iteration, ParticleContainer& pc) {
	outputWriter::XYZWriter writer;
	writer.plotParticles(pc, fileName, iteration);
}
