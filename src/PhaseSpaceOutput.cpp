#include "PhaseSpaceOutput.h"


PSO::~PSO() {

}


void PSO::openFile(){
	outFile.open ("PhaseSpaceOutput.txt");
	outFile << "X	V	F	OldF	M	Type \n";
}


void PSO::iterateFunc(Particle& p) {
	utils::Vector<double, 3>& X = p.getX();
	utils::Vector<double, 3>& V = p.getV();
	utils::Vector<double, 3>& F = p.getF();
	utils::Vector<double, 3>& OldF = p.getOldF();
	double M = p.getM();
	int Type = p.getType();

	outFile << X << "	" << V << "	" << F << "	" << OldF << "	" << M << "	" << Type << "\n";
}


void PSO::closeFile(){
	outFile.close();
}
