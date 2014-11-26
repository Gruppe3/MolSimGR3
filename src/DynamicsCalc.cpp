/*
 * DynamicsCalc.cpp
 *
 *  Created on: 26.11.2014
 *      Author: christiansturm
 */

#include "DynamicsCalc.h"

extern double delta_t;

CalcX::~CalcX() {

}

CalcV::~CalcV() {

}

void CalcX::iterateFunc(Particle& p) {
	utils::Vector<double, 3>& x = p.getX();
	x = x + delta_t * (p.getV() + delta_t / (2 * p.getM()) * p.getF());
}

void CalcV::iterateFunc(Particle& p) {
	utils::Vector<double, 3>& v = p.getV();
	v = v + delta_t / (2 * p.getM()) * (p.getOldF() + p.getF());
}
