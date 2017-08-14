/*

Copyright (C) 2017, Battelle Memorial Institute
All rights reserved.

This software was co-developed by Pacific Northwest National Laboratory, operated by the Battelle Memorial Institute; the National Renewable Energy Laboratory, operated by the Alliance for Sustainable Energy, LLC; and the Lawrence Livermore National Laboratory, operated by Lawrence Livermore National Security, LLC.

*/
#ifndef _HELICS_CORE_FACTORY_
#define _HELICS_CORE_FACTORY_


#include "helics/core/core-types.h"
#include <memory>
#include <string>

namespace helics {

class Core;
class CommonCore;
/**
 * Factory for building Core API instances.
 */

 /** available core types*/


helics_core_type coreTypeFromString(const std::string &type);

class CoreFactory {
public:
  /**
   * Creates a Core API object of the specified type.
   *
   * Invokes initialize() on the instantiated Core object.
   */
  static std::shared_ptr<Core> create (helics_core_type type, const std::string &initializationString);
  /** tries to find a named core if it fails it creates a new one
  */
  static std::shared_ptr<Core> FindOrCreate(helics_core_type type, const std::string &coreName, const std::string &initializationString);

  /**
   * Returns true if type specified is available in current compilation.
   */
  static bool isAvailable (helics_core_type type);
  /** locate a registered Core by name
  @param name the name of the core to find
  @return a shared_ptr to the testCore*/
  static std::shared_ptr<CommonCore> findCore(const std::string &name);

  /** register a testCore so it can be found by others
  @details also cleans up any leftover bCoresrokers that were previously unregistered this can be controlled by calling cleanUpBrokers
  earlier if desired
  @param tcore a pointer to a testCore object that should be found globally
  @return true if the registration was successful false otherwise*/
  static bool registerCommonCore(std::shared_ptr<CommonCore> tcore);

  /** remove a Core from the registry
  @param name the name of the Core to unregister
  */
  static void unregisterCore(const std::string &name);
  /** clean up unused cores
  @details when Cores are unregisterd they get put in a holding area that gets cleaned up when a new Core is registered
  or when the clean up function is called this prevents some odd threading issues
  */
  static void cleanUpCores();

  /** make a copy of the broker pointer to allow access to the new name
  */
  static void copyCoreIdentifier(const std::string &copyFromName, const std::string &copyToName);
};




} // namespace helics

#endif
