#include "PhaseSpaceOutput.h"


PSO::~PSO() {

}


void PSO::openFile(int num_particles){
	outFile.open ("PhaseSpaceOutput.txt");
	outFile << "# X	V	F	OldF	M	Type \n";
	outFile << num_particles << "\n";
}


void PSO::iterateFunc(Particle& p) {
	utils::Vector<double, 3>& X = p.getX();
	utils::Vector<double, 3>& V = p.getV();
	utils::Vector<double, 3>& F = p.getF();
	utils::Vector<double, 3>& OldF = p.getOldF();
	double M = p.getM();
	int Type = p.getType();

	for (int i = 0; i < 3; i++) {
		outFile << X[i] << " ";
	}
	for (int i = 0; i < 3; i++) {
		outFile << V[i] << " ";
	}
	for (int i = 0; i < 3; i++) {
		outFile << F[i] << " ";
	}
	for (int i = 0; i < 3; i++) {
		outFile << OldF[i] << " ";
	}
	outFile << M << " " << Type << "\n";
}


void PSO::closeFile(){
	outFile.close();
}
