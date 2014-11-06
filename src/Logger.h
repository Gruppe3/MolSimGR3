/*
 * Logger.h
 *
 *  Created on: 06.11.2014
 *      Author: christiansturm
 */

#ifndef SRC_LOGGER_H_
#define SRC_LOGGER_H_

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
//#include <log4cxx/helpers/exception.h>

using namespace log4cxx;
using namespace log4cxx::helpers;

const LoggerPtr molsimlog(Logger::getLogger("molsim"));
const LoggerPtr particlelog(Logger::getLogger("molsim.particle"));
const LoggerPtr forcelog(log4cxx::Logger::getLogger("molsim.particle.force"));
const LoggerPtr testlog(log4cxx::Logger::getLogger("molsim.test"));
const LoggerPtr iolog(log4cxx::Logger::getLogger("molsim.io"));


#endif /* SRC_LOGGER_H_ */
