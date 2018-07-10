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

	BOOST_CHECK(!mFed->hasMessage(p2));
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
	filter_op->setString("condition", "end"); //match all messages with a destination endpoint stating with "end"
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
	BOOST_CHECK(!mFed->hasMessage(p2));
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
	filter_op->setString("condition", "test"); //match all messages with a destination endpoint starting with "test"

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
	double drop_prob = 0.75;
	Filt->set("dropprob", drop_prob);

	fFed->enterExecutionStateAsync();
	mFed->enterExecutionState();
	fFed->enterExecutionStateComplete();

	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::execution);
	helics::data_block data(100, 'a');

	double timestep = 0.0; // 1 second
	int max_iterations = 200;
	int dropped = 0;
	for (int i = 0; i < max_iterations; i++) {
		mFed->sendMessage(p1, "port2", data);
		timestep += 1.0;
		mFed->requestTime(timestep);
		// Check if message is received
		if (!mFed->hasMessage(p2)) {
			dropped++;

		}
		else
		{
			mFed->getMessage(p2);
		}
	}
	auto iterations = static_cast<double>(max_iterations);
	double pest = static_cast<double>(dropped) / iterations;
	//this should result in an expected error of 1 in 10K tests
	double ebar = 4.0*std::sqrt(drop_prob*(1.0 - drop_prob) / iterations);

	BOOST_CHECK_GE(pest, drop_prob - ebar);
	BOOST_CHECK_LE(pest, drop_prob + ebar);
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
	double prob = 0.45;
	op->set("prob", prob);
	fFed->setFilterOperator(f1, op->getOperator());

	fFed->enterExecutionStateAsync();
	mFed->enterExecutionState();
	fFed->enterExecutionStateComplete();

	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::execution);
	helics::data_block data(100, 'a');

	double timestep = 0.0; // 1 second
	int max_iterations = 150;
	int count = 0;
	for (int i = 0; i < max_iterations; i++) {
		mFed->sendMessage(p1, "port2", data);
		timestep += 1.0;
		mFed->requestTime(timestep);
		// Check if message is received
		if (mFed->hasMessage(p2)) {
			count++;
			mFed->getMessage(p2);
		}
	}
	auto iterations = static_cast<double>(max_iterations);
	double pest = 1.0 - static_cast<double>(count) / iterations;
	//this should result in an expected error of 1 in 10K tests
	double ebar = 4.0*std::sqrt(prob*(1.0 - prob) / iterations);

	BOOST_CHECK_GE(pest, prob - ebar);
	BOOST_CHECK_LE(pest, prob + ebar);
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
	double drop_prob = 0.25;
	Filt->set("dropprob", drop_prob);

	fFed->enterExecutionStateAsync();
	mFed->enterExecutionState();
	fFed->enterExecutionStateComplete();

	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::execution);
	helics::data_block data(100, 'a');

	double timestep = 0.0; // 1 second
	int max_iterations = 150;
	int dropped = 0;
	for (int i = 0; i < max_iterations; i++) {
		mFed->sendMessage(p1, "port2", data);
		timestep += 1.0;
		mFed->requestTime(timestep);
		// Check if message is received
		if (!mFed->hasMessage(p2)) {
			dropped++;
		}
		else
		{
			//purposely dropping the messages
			mFed->getMessage(p2);
		}
	}

	auto iterations = static_cast<double>(max_iterations);
	double pest = static_cast<double>(dropped) / iterations;
	//this should result in an expected error of 1 in 10K tests
	double ebar = 4.0*std::sqrt(drop_prob*(1.0 - drop_prob) / iterations);

	BOOST_CHECK_GE(pest, drop_prob - ebar);
	BOOST_CHECK_LE(pest, drop_prob + ebar);
	mFed->finalize();
	fFed->finalize();
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
	double prob = 0.1;
	op->set("prob", prob);
	fFed->setFilterOperator(f1, op->getOperator());

	fFed->enterExecutionStateAsync();
	mFed->enterExecutionState();
	fFed->enterExecutionStateComplete();

	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::execution);
	helics::data_block data(500, 'a');

	double timestep = 0.0; // 1 second
	int max_iterations = 150;
	int count = 0;
	for (int i = 0; i < max_iterations; i++) {
		mFed->sendMessage(p1, "port2", data);
		timestep++;
		mFed->requestTime(timestep);
		if (mFed->hasMessage(p2)) {
			count++;
			mFed->getMessage(p2);
		}
	}
	auto iterations = static_cast<double>(max_iterations);
	double pest = 1.0 - static_cast<double>(count) / iterations;
	//this should result in an expected error of 1 in 10K tests
	double ebar = 4.0*std::sqrt(prob*(1.0 - prob) / iterations);

	BOOST_CHECK_GE(pest, prob - ebar);
	BOOST_CHECK_LE(pest, prob + ebar);
	mFed->finalize();
	fFed->finalize();
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

	auto Filt = helics::make_source_filter(helics::defined_filter_types::randomDelay, fFed.get(), "port1", "filter1");
	Filt->setString("distribution", "binomial");

	Filt->set("param1", 4); //max_delay=4
	Filt->set("param2", 0.5); //prob

	fFed->enterExecutionStateAsync();
	mFed->enterExecutionState();
	fFed->enterExecutionStateComplete();

	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::execution);
	helics::data_block data(100, 'a');
	mFed->sendMessage(p1, "port2", data);

	double timestep = 0.0; // 1 second
	int max_iterations = 4;
	int count = 0;
	double actual_delay = 100.0;

	for (int i = 0; i < max_iterations; i++) {
		timestep += 1.0;
		mFed->requestTime(timestep);
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

	mFed->finalize();
	fFed->finalize();
	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::finalize);
}

/**
Test random delay filter
This test case tests random delay filter for two type of distributions - geometric and uniform.
The filter is applied to source endpoint.
*/
BOOST_DATA_TEST_CASE(message_random_delay_geometric_uniform, bdata::make(core_types_single), core_type)
{
	auto broker = AddBroker(core_type, 2);
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "source");
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "geo");

	auto fFed = GetFederateAs<helics::MessageFederate>(0);
	auto mFed = GetFederateAs<helics::MessageFederate>(1);
	auto uFed = GetFederateAs<helics::MessageFederate>(1);

	auto p1 = mFed->registerGlobalEndpoint("port1");
	auto p2 = mFed->registerGlobalEndpoint("port2");
	auto p3 = mFed->registerGlobalEndpoint("port3");

	auto geoFilt = helics::make_source_filter(helics::defined_filter_types::randomDelay, fFed.get(), "port1", "filter1");
	geoFilt->setString("distribution", "geometric");
	geoFilt->set("param1", 0.7); //prob

	auto uniFilt = helics::make_source_filter(helics::defined_filter_types::randomDelay, fFed.get(), "port1", "filter2");
	uniFilt->setString("distribution", "uniform");
	uniFilt->set("min", 0.0); //min
	uniFilt->set("max", 1.0); //max

	fFed->enterExecutionStateAsync();
	mFed->enterExecutionState();
	fFed->enterExecutionStateComplete();

	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::execution);
	helics::data_block data(100, 'a');
	helics::data_block data2(100, 'x');
	mFed->sendMessage(p1, "port2", data);
	mFed->sendMessage(p1, "port3", data2);

	double timestep = 0.0; // 1 second
	int max_iterations = 4;
	int count1 = 0;
	int count2 = 0;
	double actual_delay1 = 100.0;
	double actual_delay2 = 100.0;

	for (int i = 0; i < max_iterations; i++) {
		timestep += 1.0;
		mFed->requestTime(timestep);
		// Check if message is received
		if (mFed->hasMessage(p2)) {
			auto m2 = mFed->getMessage(p2);
			BOOST_CHECK_EQUAL(m2->source, "port1");
			BOOST_CHECK_EQUAL(m2->dest, "port2");
			BOOST_CHECK_EQUAL(m2->data.size(), data.size());
			actual_delay1 = m2->time;
			count1++;
		}
		// Check if message is received
		if (mFed->hasMessage(p3)) {
			auto m3 = mFed->getMessage(p3);
			BOOST_CHECK_EQUAL(m3->source, "port1");
			BOOST_CHECK_EQUAL(m3->dest, "port3");
			BOOST_CHECK_EQUAL(m3->data.size(), data.size());
			actual_delay2 = m3->time;
			count2++;
		}
	}

	BOOST_CHECK_EQUAL(count1, 1);
	BOOST_CHECK(actual_delay1 <= 4);
	BOOST_CHECK_EQUAL(count2, 1);
	BOOST_CHECK(actual_delay2 <= 4);

	mFed->finalize();
	fFed->finalize();
	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::finalize);
}


/**
Test random delay filter
This test case tests random delay filter for two type of
distributions - normal and poisson. The filter is applied to source endpoint.
*/
BOOST_DATA_TEST_CASE(message_random_delay_normal_poisson_exponential,
	bdata::make(core_types_single), core_type)
{
	auto broker = AddBroker(core_type, 2);
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "source");
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "geo");

	auto fFed = GetFederateAs<helics::MessageFederate>(0);
	auto mFed = GetFederateAs<helics::MessageFederate>(1);
	auto uFed = GetFederateAs<helics::MessageFederate>(1);

	auto p1 = mFed->registerGlobalEndpoint("port1");
	auto p2 = mFed->registerGlobalEndpoint("port2");
	auto p3 = mFed->registerGlobalEndpoint("port3");

	auto normFilt = helics::make_source_filter(
		helics::defined_filter_types::randomDelay,
		fFed.get(), "port1", "normal");
	normFilt->setString("distribution", "normal");
	normFilt->set("mean", 5.0); //mean
	normFilt->set("stddev", 2.0); //stddev

	auto poissonFilt = helics::make_source_filter(
		helics::defined_filter_types::randomDelay,
		fFed.get(), "port1", "poisson");
	poissonFilt->setString("distribution", "poisson");
	poissonFilt->set("mean", 2.1); //mean

	fFed->enterExecutionStateAsync();
	mFed->enterExecutionState();
	fFed->enterExecutionStateComplete();

	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::execution);
	helics::data_block data(100, 'a');
	helics::data_block data2(std::string("This is poisson distribution"));
	mFed->sendMessage(p1, "port2", data);
	mFed->sendMessage(p1, "port3", data2);

	double timestep = 0.0; // 1 second
	int max_iterations = 4;
	int count1 = 0;
	int count2 = 0;
	double actual_delay1 = 100.0;
	double actual_delay2 = 100.0;

	for (int i = 0; i < max_iterations; i++) {
		timestep += 1.0;
		mFed->requestTime(timestep);
		// Check if message is received
		if (mFed->hasMessage(p2)) {
			auto m2 = mFed->getMessage(p2);
			BOOST_CHECK_EQUAL(m2->source, "port1");
			BOOST_CHECK_EQUAL(m2->dest, "port2");
			BOOST_CHECK_EQUAL(m2->data.size(), data.size());
			actual_delay1 = m2->time;
			count1++;
		}
		// Check if message is received
		if (mFed->hasMessage(p3)) {
			auto m3 = mFed->getMessage(p3);
			BOOST_CHECK_EQUAL(m3->source, "port1");
			BOOST_CHECK_EQUAL(m3->dest, "port3");
			BOOST_CHECK_EQUAL(m3->data.size(), data2.size());
			actual_delay2 = m3->time;
			count2++;
		}
	}

	BOOST_CHECK_EQUAL(count1, 1);
	BOOST_CHECK(actual_delay1 <= 4);
	BOOST_CHECK_EQUAL(count2, 1);
	BOOST_CHECK(actual_delay2 <= 4);

	mFed->finalize();
	fFed->finalize();
	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::finalize);
}

/**
Test custom filter
This test case uses MessageDataOperator as operator for custom filter. It manipulates the
data in the message.
*/
BOOST_DATA_TEST_CASE(message_custom_data, bdata::make(core_types_single), core_type)
{
	auto broker = AddBroker(core_type, 2);
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "filter");
	AddFederates<helics::MessageFederate>(core_type, 1, broker, 1.0, "message");

	auto fFed = GetFederateAs<helics::MessageFederate>(0);
	auto mFed = GetFederateAs<helics::MessageFederate>(1);

	auto p1 = mFed->registerGlobalEndpoint("port1");
	auto p2 = mFed->registerGlobalEndpoint("port2");

	auto Filt = helics::make_source_filter(helics::defined_filter_types::custom, fFed.get(), "port1", "filter1");
	auto dataOperator = std::make_shared<helics::MessageDataOperator>();
	dataOperator->setDataFunction([](helics::data_view data_in) {
		const char *str = "this is a test string";
		data_in = str;
		return data_in; });
	fFed->setFilterOperator(fFed->getSourceFilterId("filter1"), dataOperator);

	fFed->enterExecutionStateAsync();
	mFed->enterExecutionState();
	fFed->enterExecutionStateComplete();

	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::execution);
	helics::data_block data(500, 'a');
	mFed->sendMessage(p1, "port2", data);

	mFed->requestTimeAsync(1.0);
	fFed->requestTime(1.0);
	mFed->requestTimeComplete();

	BOOST_REQUIRE(mFed->hasMessage(p2));
	helics::data_block expected(std::string("this is a test string"));

	auto m2 = mFed->getMessage(p2);
	BOOST_CHECK_EQUAL(m2->source, "port1");
	BOOST_CHECK_EQUAL(m2->dest, "port2");
	BOOST_CHECK_EQUAL(m2->data.size(), expected.size());

	fFed->requestTimeAsync(2.0);
	mFed->requestTime(2.0);
	fFed->requestTimeComplete();

	mFed->finalize();
	fFed->finalize();
	BOOST_CHECK(fFed->getCurrentState() == helics::Federate::op_states::finalize);
}

BOOST_AUTO_TEST_CASE(test_empty)
{

}
BOOST_AUTO_TEST_SUITE_END()