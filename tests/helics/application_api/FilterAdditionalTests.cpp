/*
Copyright � 2017-2018,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance for Sustainable Energy, LLC
All rights reserved. See LICENSE file and DISCLAIMER for more details.
*/

#include "helics/application_api/Federate.hpp"
#include "helics/application_api/Filters.hpp"
#include "helics/application_api/MessageOperators.hpp"
#include "helics/application_api/FilterOperations.hpp"
#include "testFixtures.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <future>
/** these test cases test out the message federates
*/

BOOST_FIXTURE_TEST_SUITE(additional_filter_tests, FederateTestFixture)

namespace bdata = boost::unit_test::data;

namespace utf = boost::unit_test;


/**
Test rerouter filter
This test case sets reroute filter on a source endpoint. Ths means message
sent from this endpoint will be rerouted to a new destination endpoint.
*/

BOOST_DATA_TEST_CASE(message_reroute_filter_object1, bdata::make(core_types), core_type)
{
	auto broker = AddBroker(core_type, 2);
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "filter");
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "message");

	auto fFed = GetFederateAs<helics::MessageFederate>(0);
	auto mFed = GetFederateAs<helics::MessageFederate>(1);

	auto p1 = mFed->registerGlobalEndpoint("port1");
	auto p2 = mFed->registerGlobalEndpoint("port2");
	auto p3 = mFed->registerGlobalEndpoint("port3");

	auto Filt = helics::make_source_filter(helics::defined_filter_types::reroute, fFed.get(), "port1", "filter1");
	Filt->setString("newdestination", "port3");

	fFed->enterExecutionStateAsync();
	mFed->enterExecutionState();
	fFed->enterExecutionStateComplete();

	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::execution);
	helics::data_block data(500, 'a');
	mFed->sendMessage(p1, "port2", data);

	mFed->requestTimeAsync(1.0);
	fFed->requestTime(1.0);
	mFed->requestTimeComplete();

	BOOST_REQUIRE(mFed->hasMessage(p3));

	auto m2 = mFed->getMessage(p3);
	BOOST_CHECK_EQUAL(m2->source, "port1");
	BOOST_CHECK_EQUAL(m2->original_dest, "port2");
	BOOST_CHECK_EQUAL(m2->dest, "port3");
	BOOST_CHECK_EQUAL(m2->data.size(), data.size());

	fFed->requestTimeAsync(2.0);
	mFed->requestTime(2.0);
	fFed->requestTimeComplete();

	mFed->finalize();
	fFed->finalize();
	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::finalize);
}

/**
Test rerouter filter under condition
This test case sets reroute filter on a source endpoint with a condition parameter. 
Ths means message sent from this endpoint will be rerouted to a new destination 
endpoint only if condition matches.
*/

BOOST_DATA_TEST_CASE(message_reroute_filter_condition, bdata::make(core_types), core_type)
{
	auto broker = AddBroker(core_type, 2);
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "filter");
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "message");

	auto fFed = GetFederateAs<helics::MessageFederate>(0);
	auto mFed = GetFederateAs<helics::MessageFederate>(1);

	auto p1 = mFed->registerGlobalEndpoint("port1");
	auto p2 = mFed->registerGlobalEndpoint("endpt2");
	auto p3 = mFed->registerGlobalEndpoint("port3");

	auto f1 = fFed->registerSourceFilter("filter1", "port1");
	auto filter_op = std::make_shared<helics::RerouteFilterOperation>();
	filter_op->setString("newdestination", "port3");
	fFed->setFilterOperator(f1, filter_op->getOperator());

	fFed->enterExecutionStateAsync();
	mFed->enterExecutionState();
	fFed->enterExecutionStateComplete();

	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::execution);
	helics::data_block data(500, 'a');
	mFed->sendMessage(p1, "endpt2", data);

	mFed->requestTimeAsync(1.0);
	fFed->requestTime(1.0);
	mFed->requestTimeComplete();

	BOOST_REQUIRE(mFed->hasMessage(p3));
	auto m2 = mFed->getMessage(p3);

	BOOST_CHECK_EQUAL(m2->source, "port1");
	BOOST_CHECK_EQUAL(m2->dest, "port3");
	BOOST_CHECK_EQUAL(m2->data.size(), data.size());

	fFed->requestTimeAsync(2.0);
	mFed->requestTime(2.0);
	fFed->requestTimeComplete();

	mFed->finalize();
	fFed->finalize();
	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::finalize);
}

/**
Test rerouter filter
This test case sets reroute filter on a destination endpoint. Ths means message
sent to this endpoint will be rerouted to a new destination endpoint.
*/

BOOST_DATA_TEST_CASE(message_reroute_filter_object2, bdata::make(core_types), core_type)
{
	auto broker = AddBroker(core_type, 2);
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "filter");
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "message");

	auto fFed = GetFederateAs<helics::MessageFederate>(0);
	auto mFed = GetFederateAs<helics::MessageFederate>(1);

	auto p1 = mFed->registerGlobalEndpoint("port1");
	auto p2 = mFed->registerGlobalEndpoint("port2");
	auto p3 = mFed->registerGlobalEndpoint("port3");

    auto f1 = fFed->registerSourceFilter("filter1", "port1");
    auto filter_op = std::make_shared<helics::RerouteFilterOperation>();
    filter_op->setString("newdestination", "port3");
    filter_op->setString("condition", "test.*"); //match all messages with a destination endpoint starting with "test"

    fFed->setFilterOperator(f1, filter_op->getOperator());

	fFed->enterExecutionStateAsync();
	mFed->enterExecutionState();
	fFed->enterExecutionStateComplete();

	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::execution);
	helics::data_block data(500, 'a');
	mFed->sendMessage(p1, "port2", data);

	mFed->requestTimeAsync(1.0);
	fFed->requestTime(1.0);
	mFed->requestTimeComplete();
    //this one was delivered to the original destination
	BOOST_REQUIRE(mFed->hasMessage(p2));
	
    //this message should be delivered to the rerouted destination
	mFed->sendMessage(p1, "test324525", data);

	mFed->requestTimeAsync(2.0);
	fFed->requestTime(2.0);
	mFed->requestTimeComplete();

	BOOST_REQUIRE(mFed->hasMessage(p3));

	auto m2 = mFed->getMessage(p3);
	BOOST_CHECK_EQUAL(m2->source, "port1");
	BOOST_CHECK_EQUAL(m2->dest, "port3");
	BOOST_CHECK_EQUAL(m2->data.size(), data.size());

	mFed->finalize();
	fFed->finalize();
	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::finalize);
}

/**
Test random drop filter
This test case sets random drop filter on a source endpoint with a particular
message drop probability. This means messages may be dropped randomly with a
probability of 0.75.
*/

BOOST_DATA_TEST_CASE(message_random_drop_object, bdata::make(core_types), core_type)
{
	auto broker = AddBroker(core_type, 2);
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "filter");
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "message");

	auto fFed = GetFederateAs<helics::MessageFederate>(0);
	auto mFed = GetFederateAs<helics::MessageFederate>(1);

	auto p1 = mFed->registerGlobalEndpoint("port1");
	auto p2 = mFed->registerGlobalEndpoint("port2");

	auto Filt = helics::make_source_filter(helics::defined_filter_types::randomDrop, fFed.get(), "port1", "filter1");
	float drop_prob = 0.75;
	Filt->set("dropprob", drop_prob);

	fFed->enterExecutionStateAsync();
	mFed->enterExecutionState();
	fFed->enterExecutionStateComplete();

	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::execution);
	helics::data_block data(500, 'a');

	int timestep = 0.0; // 1 second
	int max_iterations = 12;
	int dropped = 0;
	for (int i = 0; i < max_iterations; i++) {
		mFed->sendMessage(p1, "port2", data);
		timestep++;
		mFed->requestTimeAsync(timestep);
		fFed->requestTime(timestep);
		mFed->requestTimeComplete();
		// Check if message is received
		if (!mFed->hasMessage(p2)) {
			dropped++;
		}
	}
	int expected_drop = drop_prob * max_iterations;
	BOOST_CHECK(dropped <= expected_drop);
	mFed->finalize();
	fFed->finalize();
	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::finalize);
}

/**
Test random drop filter
This test case sets random drop filter on a source endpoint with a particular
message arrival probability. This means messages may be received randomly with a
probability of 0.9.
*/

BOOST_DATA_TEST_CASE(message_random_drop_object1, bdata::make(core_types), core_type)
{
	auto broker = AddBroker(core_type, 2);
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "filter");
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "message");

	auto fFed = GetFederateAs<helics::MessageFederate>(0);
	auto mFed = GetFederateAs<helics::MessageFederate>(1);

	auto p1 = mFed->registerGlobalEndpoint("port1");
	auto p2 = mFed->registerGlobalEndpoint("port2");

	auto f1 = fFed->registerSourceFilter("filter1", "port1");
	auto op = std::make_shared<helics::RandomDropFilterOperation>();
	double prob = 0.9;
	op->set("prob", prob);
	fFed->setFilterOperator(f1, op->getOperator());

	fFed->enterExecutionStateAsync();
	mFed->enterExecutionState();
	fFed->enterExecutionStateComplete();
	
	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::execution);
	helics::data_block data(500, 'a');

	int timestep = 0.0; // 1 second
	int max_iterations = 12;
	int count = 0;
	for (int i = 0; i < max_iterations; i++) {
		mFed->sendMessage(p1, "port2", data);
		timestep++;
		mFed->requestTimeAsync(timestep);
		fFed->requestTime(timestep);
		mFed->requestTimeComplete();
		// Check if message is received
		if (mFed->hasMessage(p2)) {
			count++;
		}
	}
	int expected = prob * max_iterations;
	BOOST_CHECK(count >= expected);
	mFed->finalize();
	fFed->finalize();
	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::finalize);
}

/**
Test random drop filter
This test case sets random drop filter on a destination endpoint with a particular
message drop probability. This means messages may be dropped randomly with a
probability of 0.75.
*/
BOOST_DATA_TEST_CASE(message_random_drop_dest_object, bdata::make(core_types), core_type)
{
	auto broker = AddBroker(core_type, 2);
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "filter");
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "message");

	auto fFed = GetFederateAs<helics::MessageFederate>(0);
	auto mFed = GetFederateAs<helics::MessageFederate>(1);

	auto p1 = mFed->registerGlobalEndpoint("port1");
	auto p2 = mFed->registerGlobalEndpoint("port2");

	auto Filt = helics::make_destination_filter(helics::defined_filter_types::randomDrop, 
				fFed.get(), "port2", "filter1");
	float drop_prob = 0.75;
	Filt->set("dropprob", drop_prob);

	fFed->enterExecutionStateAsync();
	mFed->enterExecutionState();
	fFed->enterExecutionStateComplete();
	
	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::execution);
	helics::data_block data(500, 'a');

	int timestep = 0.0; // 1 second
	int max_iterations = 12;
	int dropped = 0;
	for (int i = 0; i < max_iterations; i++) {
		mFed->sendMessage(p1, "port2", data);
		timestep++;
		fFed->requestTimeAsync(timestep);
		mFed->requestTime(timestep);
		fFed->requestTimeComplete();
		// Check if message is received
		if (!mFed->hasMessage(p2)) {
			dropped++;
		}
	}
	int expected = drop_prob * max_iterations;
	BOOST_CHECK(dropped >= expected);
}

/**
Test random drop filter
This test case sets random drop filter on a destination endpoint with a particular
message arrival probability. This means messages may be received randomly with a
probability of 0.9.
*/
BOOST_DATA_TEST_CASE(message_random_drop_dest_object1, bdata::make(core_types), core_type)
{
	auto broker = AddBroker(core_type, 2);
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "filter");
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "message");

	auto fFed = GetFederateAs<helics::MessageFederate>(0);
	auto mFed = GetFederateAs<helics::MessageFederate>(1);

	auto p1 = mFed->registerGlobalEndpoint("port1");
	auto p2 = mFed->registerGlobalEndpoint("port2");

	auto f1 = fFed->registerDestinationFilter("filter1", "port2");
	auto op = std::make_shared<helics::RandomDropFilterOperation>();
	double prob = 0.9;
	op->set("prob", prob);
	fFed->setFilterOperator(f1, op->getOperator());

	fFed->enterExecutionStateAsync();
	mFed->enterExecutionState();
	fFed->enterExecutionStateComplete();
	
	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::execution);
	helics::data_block data(500, 'a');

	double timestep = 0.0; // 1 second
	int max_iterations = 12;
	int count = 0;
	for (int i = 0; i < max_iterations; i++) {
		mFed->sendMessage(p1, "port2", data);
		timestep++;
		mFed->requestTimeAsync(timestep);
		fFed->requestTime(timestep);
		mFed->requestTimeComplete();
		// Check if message is received
		if (mFed->hasMessage(p2)) {
			count++;
		}
	}
	int success_count = prob * max_iterations;
	BOOST_CHECK(count >= success_count);
}

/**
Test random delay filter
This test case sets random delay filter on a source endpoint. 
This means messages may be delayed by random delay based on
binomial distribution.
*/
BOOST_DATA_TEST_CASE(message_random_delay_object, bdata::make(core_types), core_type)
{
	auto broker = AddBroker(core_type, 2);
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "filter");
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "message");

	auto fFed = GetFederateAs<helics::MessageFederate>(0);
	auto mFed = GetFederateAs<helics::MessageFederate>(1);

	auto p1 = mFed->registerGlobalEndpoint("port1");
	auto p2 = mFed->registerGlobalEndpoint("port2");

	fFed->enterExecutionStateAsync();
	mFed->enterExecutionState();
	fFed->enterExecutionStateComplete();

	auto Filt = helics::make_source_filter(helics::defined_filter_types::randomDelay, fFed.get(), "port1", "filter1");
	Filt->setString("distribution", "binomial");
	
	Filt->set("param1", 4); //max_delay=4
	Filt->set("param2", 0.5); //prob

	fFed->enterExecutionStateAsync();
	mFed->enterExecutionState();
	fFed->enterExecutionStateComplete();

	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::execution);
	helics::data_block data(500, 'a');
	mFed->sendMessage(p1, "port2", data);

	double timestep = 0.0; // 1 second
	int max_iterations = 4;
	int count = 0;
	double actual_delay = 100.0;

	for (int i = 0; i < max_iterations; i++) {
		timestep++;
		mFed->requestTimeAsync(timestep);
		fFed->requestTime(timestep);
		mFed->requestTimeComplete();
		// Check if message is received
		if (mFed->hasMessage(p2)) {
			auto m2 = mFed->getMessage(p2);
			BOOST_CHECK_EQUAL(m2->source, "port1");
			BOOST_CHECK_EQUAL(m2->dest, "port2");
			BOOST_CHECK_EQUAL(m2->data.size(), data.size());
			actual_delay = m2->time;
			count++;
		}
	}
	BOOST_CHECK_EQUAL(count, 1);
	BOOST_CHECK(actual_delay <= 4);

	mFed->requestTime(5.0);
	fFed->requestTimeComplete();
	mFed->finalize();
	fFed->finalize();
	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::finalize);
}

BOOST_AUTO_TEST_CASE(test_empty)
{

}
BOOST_AUTO_TEST_SUITE_END()