/*
 * FileReader.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#ifndef FILE_READER_H_
#define FILE_READER_H_

#include "Particle.h"
#include "ParticleContainer.h"
#include <list>

class FileReader {

public:
	FileReader();
	virtual ~FileReader();

	void readFile(ParticleContainer& particleContainer, char* filename);

};


#endif /* FILE_READER_H_ */
