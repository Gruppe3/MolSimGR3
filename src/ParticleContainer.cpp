/*
 * ParticleContainer.cpp
 *
 *  Created on: 20.10.2014
 *      Author: christiansturm
 */

#include "ParticleContainer.h"
#include "Logger.h"

#include <iostream>
#include <cstring>
#include <cstdlib>

const LoggerPtr particlelog(log4cxx::Logger::getLogger("molsim.particle"));

ParticleContainer::ParticleContainer() {
	particles = std::list<Particle>();
	resetIterator();
}

ParticleContainer::ParticleContainer(list<Particle> l) {
	particles = l;
	resetIterator();
}

ParticleContainer::ParticleContainer(const ParticleContainer& pc) {
	particles = pc.particles;
	iterator = pc.iterator;
	othersIterator = pc.othersIterator;
}

ParticleContainer::~ParticleContainer() {
	LOG4CXX_TRACE(particlelog, "ParticleContainer destructed!");
}

void ParticleContainer::add(Particle& p) {
	particles.push_back(p);
	resetIterator();
}

int ParticleContainer::size() {
	return particles.size();
}

bool ParticleContainer::hasNext() {
	return iterator != particles.end();
}

Particle& ParticleContainer::next() {
	Particle& p = *iterator;
	iterator++;
	othersIterator = iterator;
	return p;
}

bool ParticleContainer::hasNextOther() {
	return othersIterator != particles.end();
}

Particle& ParticleContainer::nextOther() {
	Particle& p = *othersIterator;
	othersIterator++;
	return p;
}

void ParticleContainer::resetIterator() {
	iterator = particles.begin();
	std::list<Particle>::iterator tmp(iterator);
	tmp++;
	othersIterator = tmp;
}

void ParticleContainer::iterate(PCApply *fnc) {
	for (iterator = particles.begin(); iterator != particles.end();
			iterator++) {
		Particle& p = *iterator;
		fnc->iterateFunc(p);
	}
}

void ParticleContainer::iteratePair(PCApply *fnc) {
	for (iterator = particles.begin(); iterator != particles.end(); iterator++) {
		Particle& p1 = *iterator;
		othersIterator = iterator;
		for (++othersIterator; othersIterator != particles.end(); othersIterator++) {
			Particle& p2 = *othersIterator;
			fnc->iteratePairFunc(p1, p2);
		}
	}
}

void ParticleContainer::iterateDirectNeighbours(PCApply *fnc) {
	for (iterator = particles.begin(); iterator != particles.end(); iterator++) {
		Particle& p1 = *iterator;
		for (int j = 0; j < 8; j++) {
			Particle& p2 = *(p1.Neighbour[j]);
			if (&p2 != NULL) {
				fnc->iteratePairFunc(p1, p2);
			}
		}
	}
}
