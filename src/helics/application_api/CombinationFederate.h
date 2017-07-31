/*

Copyright (C) 2017, Battelle Memorial Institute
All rights reserved.

This software was co-developed by Pacific Northwest National Laboratory, operated by the Battelle Memorial Institute; the National Renewable Energy Laboratory, operated by the Alliance for Sustainable Energy, LLC; and the Lawrence Livermore National Laboratory, operated by Lawrence Livermore National Security, LLC.

*/
#ifndef _COMBINATION_FEDERATE_H_
#define _COMBINATION_FEDERATE_H_
#pragma once

#include "ValueFederate.h"
#include "MessageFederate.h"
#include "identifierTypes.hpp"

namespace helics
{
	/** class defining the value based interface */
	class CombinationFederate :public ValueFederate, public MessageFederate 
	{
	public:
		CombinationFederate();
		/**constructor taking a federate information structure and using the default core
		@param[in] fi  a federate information structure
		*/
		CombinationFederate(const FederateInfo &fi);
		/**constructor taking a file with the required information
		@param[in] file a file defining the federate information
		*/
		CombinationFederate(const std::string &file);

		CombinationFederate(CombinationFederate &&fed) noexcept;

		virtual ~CombinationFederate();

		CombinationFederate &operator=(CombinationFederate &&fed) noexcept;
	protected:
		virtual void updateTime(Time newTime, Time oldTime) override;
		virtual void StartupToInitializeStateTransition() override;
		virtual void InitializeToExecuteStateTransition() override;
	public:
		virtual void registerInterfaces(const std::string &jsonString) override;
	};

}
#endif