/*
 * XYZWriter.cpp
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#include "outputWriter/XYZWriter.h"
#include "utils/Vector.h"
#include <cstdlib>
#include <sstream>

using namespace std;

namespace outputWriter {

XYZWriter::XYZWriter() {
	// TODO Auto-generated constructor stub

}

XYZWriter::~XYZWriter() {
	// TODO Auto-generated destructor stub
}

void XYZWriter::plotParticles(ParticleContainer& pc, const std::string& filename, int iteration) {
	std::ofstream file;
	stringstream strstr;
	strstr << filename << "_" << (iteration < 10 ? "000" : (iteration < 100 ? "00" : ( iteration < 1000 ? "0" : "") )) << iteration << ".xyz";

	file.open(strstr.str().c_str());
	file << pc.size() << endl;
	file << "Generated by MolSim. See http://openbabel.org/wiki/XYZ_(format) for file format doku." << endl;

	pc.resetIterator();
	while (pc.hasNext()) {
		Particle& p = pc.next();
		utils::Vector<double, 3> x = p.getX();
		file << "Ar ";
		file.setf(ios_base::showpoint);

		for (int i = 0; i < 3; i++) {
			file << x[i] << " ";
		}

		file << endl;
	}

	file.close();
}

} // end namespace
