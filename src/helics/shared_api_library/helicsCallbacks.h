/*
Copyright © 2017-2018,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance for Sustainable Energy, LLC
All rights reserved. See LICENSE file and DISCLAIMER for more details.
*/

/** @file
@brief functions dealing with callbacks for the shared library
*/

#ifndef HELICS_APISHARED_CALLBACK_FUNCTIONS_H_
#define HELICS_APISHARED_CALLBACK_FUNCTIONS_H_

#include "helics.h"

#ifdef __cplusplus
extern "C" {
#endif

/** add a logging callback to a broker
@details add a logging callback function for the C The logging callback will be called when
a message flows into a broker from the core or from a broker
@param broker the broker object in which to create a subscription must have been create with helicsCreateValueFederate or
helicsCreateCombinationFederate
@param logger a callback with signature void(int, const char *, const char *);
the function arguments are loglevel,  an identifier, and a message string
@return an object containing the subscription
*/
HELICS_EXPORT helics_status helicsBrokerAddLoggingCallback (helics_broker broker, void (*logger) (int, const char *, const char *));

/** add a logging callback to a core
@details add a logging callback function for the C The logging callback will be called when
a message flows into a core from the core or from a broker
@param core the core object in which to create a subscription must have been create with helicsCreateValueFederate or
helicsCreateCombinationFederate
@param logger a callback with signature void(int, const char *, const char *);
the function arguments are loglevel,  an identifier, and a message string
@return an object containing the subscription
*/
HELICS_EXPORT helics_status helicsCoreAddLoggingCallback (helics_core core, void (*logger) (int, const char *, const char *));

/** add a logging callback to a federate
   @details add a logging callback function for the C The logging callback will be called when
   a message flows into a federate from the core or from a federate
   @param fed the federate object in which to create a subscription must have been create with helicsCreateValueFederate or
   helicsCreateCombinationFederate
   @param logger a callback with signature void(int, const char *, const char *);
    the function arguments are loglevel,  an identifier, and a message string
   @return an object containing the subscription
   */
HELICS_EXPORT helics_status helicsFederateAddLoggingCallback (helics_federate fed, void (*logger) (int, const char *, const char *));

#ifdef __cplusplus
} /* end of extern "C" { */
#endif

#endif
