/*
 * ParticleContainer.cpp
 *
 *  Created on: 20.10.2014
 *      Author: christiansturm
 */




#include "ParticleContainer.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

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
	LOG4CXX_TRACE(Logger::getLogger("MolSim.Particle"), "ParticleContainer destructed!");
}

void ParticleContainer::add(Particle& p) {
	particles.push_back(p);
	resetIterator();
}

int ParticleContainer::size () {
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