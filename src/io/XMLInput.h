/*
 * XMLInput.h
 *
 *  Created on: 15.11.2014
 *      Author: christiansturm
 */

#ifndef SRC_IO_XMLINPUT_H_
#define SRC_IO_XMLINPUT_H_

#include "InputHandler.h"

class XMLInput : public InputHandler {
public:
	virtual ~XMLInput();

	/** reads particles and objects generating particles from xml file (see molsim-input.xsd for valid xml definitions) and delegates data to ParticleGenerator;
	 * implementation of InputHandler::getFileInput
	 * @param fileName File containing particle list
	 * @param pc ParticleContainer to store particles*/
	void getFileInput(char* fileName, ParticleContainer* pc);
};

#endif /* SRC_IO_XMLINPUT_H_ */
