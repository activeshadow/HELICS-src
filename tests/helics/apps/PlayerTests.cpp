/*
Copyright © 2017-2018,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance for Sustainable Energy, LLC
All rights reserved. See LICENSE file and DISCLAIMER for more details.
*/
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <cstdio>

#include "exeTestHelper.h"
#include "helics/application_api/Subscriptions.hpp"
#include "helics/apps/Player.hpp"
#include "helics/common/stringToCmdLine.h"
#include "helics/core/BrokerFactory.hpp"
#include <future>

namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE (player_tests, *utf::label("ci"))

BOOST_AUTO_TEST_CASE (simple_player_test)
{
    helics::FederateInfo fi ("player1");
    fi.coreType = helics::core_type::TEST;
    fi.coreName = "core1";
    fi.coreInitString = "2";
    helics::apps::Player play1 (fi);
    fi.name = "block1";
    play1.addPublication ("pub1", helics::helics_type_t::helicsDouble);
    play1.addPoint (1.0, "pub1", 0.5);
    play1.addPoint (2.0, "pub1", 0.7);
    play1.addPoint (3.0, "pub1", 0.8);

    helics::ValueFederate vfed (fi);
    helics::Subscription sub1 (&vfed, "pub1");
    auto fut = std::async (std::launch::async, [&play1]() { play1.run (); });
    vfed.enterExecutionState ();
    auto retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 1.0);
    auto val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.5);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 2.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.7);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 3.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.8);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 5.0);
    vfed.finalize ();
    fut.get ();
}

BOOST_AUTO_TEST_CASE (simple_player_test_diff_inputs)
{
    helics::FederateInfo fi ("player1");
    fi.coreType = helics::core_type::TEST;
    fi.coreName = "core1";
    fi.coreInitString = "2";
    helics::apps::Player play1 (fi);
    fi.name = "block1";
    play1.addPublication ("pub1", helics::helics_type_t::helicsDouble);
    play1.addPoint (1.0, "pub1", "v[3.0,4.0]");
    play1.addPoint (2.0, "pub1", "0.7");
    play1.addPoint (3.0, "pub1", std::complex<double>(0.0,0.8));
    play1.addPoint (4.0, "pub1", "c[3.0+0j, 0.0-4.0j]");
    helics::ValueFederate vfed (fi);
    helics::Subscription sub1 (&vfed, "pub1");
    auto fut = std::async (std::launch::async, [&play1]() { play1.run (); });
    vfed.enterExecutionState ();
    auto retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 1.0);
    auto val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 5.0);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 2.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.7);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 3.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.8);

	retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 4.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 5.0);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 5.0);
    vfed.finalize ();
    fut.get ();
}

BOOST_AUTO_TEST_CASE (simple_player_test_iterative)
{
    helics::FederateInfo fi ("player1");
    fi.coreType = helics::core_type::TEST;
    fi.coreName = "core1";
    fi.coreInitString = "2";
    helics::apps::Player play1 (fi);
    fi.name = "block1";
    play1.addPublication ("pub1", helics::helics_type_t::helicsDouble);
    play1.addPoint (1.0, 0, "pub1", 0.5);
    play1.addPoint (1.0, 1, "pub1", 0.7);
    play1.addPoint (1.0, 2, "pub1", 0.8);

    helics::ValueFederate vfed (fi);
    helics::Subscription sub1 (&vfed, "pub1");
    auto fut = std::async (std::launch::async, [&play1]() { play1.run (); });
    vfed.enterExecutionState ();
    auto retTime = vfed.requestTimeIterative (5, helics::iteration_request::iterate_if_needed);
    BOOST_CHECK_EQUAL (retTime.grantedTime, 1.0);
    BOOST_CHECK (retTime.state == helics::iteration_result::next_step);
    auto val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.5);

    retTime = vfed.requestTimeIterative (5, helics::iteration_request::iterate_if_needed);
    BOOST_CHECK_EQUAL (retTime.grantedTime, 1.0);
    BOOST_CHECK (retTime.state == helics::iteration_result::iterating);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.7);

    retTime = vfed.requestTimeIterative (5, helics::iteration_request::iterate_if_needed);
    BOOST_CHECK_EQUAL (retTime.grantedTime, 1.0);
    BOOST_CHECK (retTime.state == helics::iteration_result::iterating);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.8);

    retTime = vfed.requestTimeIterative (5, helics::iteration_request::iterate_if_needed);
    BOOST_CHECK_EQUAL (retTime.grantedTime, 5.0);
    BOOST_CHECK (retTime.state == helics::iteration_result::next_step);
    vfed.finalize ();
    fut.get ();
}

BOOST_AUTO_TEST_CASE (simple_player_test2)
{
    helics::FederateInfo fi ("player1");
    fi.coreType = helics::core_type::TEST;
    fi.coreName = "core2";
    fi.coreInitString = "2";
    helics::apps::Player play1 (fi);
    fi.name = "block1";
    play1.addPublication<double> ("pub1");
    play1.addPoint (1.0, "pub1", 0.5);
    play1.addPoint (2.0, "pub1", 0.7);
    play1.addPoint (3.0, "pub1", 0.8);

    play1.addPublication<double> ("pub2");
    play1.addPoint (1.0, "pub2", 0.4);
    play1.addPoint (2.0, "pub2", 0.6);
    play1.addPoint (3.0, "pub2", 0.9);

    helics::ValueFederate vfed (fi);
    helics::Subscription sub1 (&vfed, "pub1");
    helics::Subscription sub2 (&vfed, "pub2");
    auto fut = std::async (std::launch::async, [&play1]() { play1.run (); });
    vfed.enterExecutionState ();

    auto retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 1.0);
    auto val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.5);
    val = sub2.getValue<double> ();
    BOOST_CHECK_CLOSE (val, 0.4, 0.000001);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 2.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.7);
    val = sub2.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.6);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 3.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.8);
    val = sub2.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.9);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 5.0);
    vfed.finalize ();
    fut.get ();
}

const std::vector<std::string> simple_files{"example1.player", "example2.player", "example3.player",
                                            "example4.player", "example5.json", "example5.player"};

BOOST_DATA_TEST_CASE (simple_player_test_files, boost::unit_test::data::make (simple_files), file)
{
    static char indx = 'a';
    helics::FederateInfo fi ("player1");
    fi.coreType = helics::core_type::TEST;
    fi.coreName = "core3";
    fi.coreName.push_back (indx++);
    fi.coreInitString = "2";
    helics::apps::Player play1 (fi);
    fi.name = "block1";
    play1.loadFile (std::string (TEST_DIR) + "/test_files/" + file);

    helics::ValueFederate vfed (fi);
    helics::Subscription sub1 (&vfed, "pub1");
    helics::Subscription sub2 (&vfed, "pub2");
    auto fut = std::async (std::launch::async, [&play1]() { play1.run (); });
    vfed.enterExecutionState ();
    auto val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.3);

    auto retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 1.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.5);
    val = sub2.getValue<double> ();
    BOOST_CHECK_CLOSE (val, 0.4, 0.000001);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 2.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.7);
    val = sub2.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.6);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 3.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.8);
    val = sub2.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.9);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 5.0);
    vfed.finalize ();
    fut.get ();
}

BOOST_AUTO_TEST_CASE (simple_player_mlinecomment)
{
    static char indx = 'a';
    helics::FederateInfo fi ("player1");
    fi.coreType = helics::core_type::TEST;
    fi.coreName = "core3";
    fi.coreName.push_back (indx++);
    fi.coreInitString = "2";
    helics::apps::Player play1 (fi);
    fi.name = "block1";
    play1.loadFile (std::string (TEST_DIR) + "/test_files/example_comments.player");

	
    BOOST_CHECK_EQUAL (play1.pointCount (), 7);
    helics::ValueFederate vfed (fi);
    helics::Subscription sub1 (&vfed, "pub1");
    helics::Subscription sub2 (&vfed, "pub2");
    auto fut = std::async (std::launch::async, [&play1]() { play1.run (); });
    vfed.enterExecutionState ();
    auto val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.3);

    auto retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 1.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.5);
    val = sub2.getValue<double> ();
    BOOST_CHECK_CLOSE (val, 0.4, 0.000001);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 2.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.7);
    val = sub2.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.6);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 3.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.8);
    val = sub2.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.9);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 5.0);
    vfed.finalize ();
    fut.get ();
    BOOST_CHECK_EQUAL (play1.publicationCount (), 2);
}

BOOST_DATA_TEST_CASE (simple_player_test_files_cmdline, boost::unit_test::data::make (simple_files), file)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    auto brk = helics::BrokerFactory::create (helics::core_type::IPC, "ipc_broker", "2");
    brk->connect ();
    std::string exampleFile = std::string (TEST_DIR) + "/test_files/" + file;

    StringToCmdLine cmdArg ("--name=player --broker=ipc_broker --core=ipc " + exampleFile);

    helics::apps::Player play1 (cmdArg.getArgCount (), cmdArg.getArgV ());

    helics::FederateInfo fi ("obj");
    fi.coreType = helics::core_type::IPC;
    fi.coreInitString = "1 --broker=ipc_broker";

    helics::ValueFederate vfed (fi);
    helics::Subscription sub1 (&vfed, "pub1");
    helics::Subscription sub2 (&vfed, "pub2");
    auto fut = std::async (std::launch::async, [&play1]() { play1.run (); });
    vfed.enterExecutionState ();
    auto val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.3);

    auto retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 1.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.5);
    val = sub2.getValue<double> ();
    BOOST_CHECK_CLOSE (val, 0.4, 0.000001);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 2.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.7);
    val = sub2.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.6);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 3.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.8);
    val = sub2.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.9);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 5.0);
    vfed.finalize ();
    fut.get ();
    brk = nullptr;
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

#ifndef DISABLE_SYSTEM_CALL_TESTS
BOOST_DATA_TEST_CASE (simple_player_test_files_ext, boost::unit_test::data::make (simple_files), file)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    exeTestRunner playerExe (std::string(HELICS_INSTALL_LOC),std::string (HELICS_BUILD_LOC) + "/apps/", "helics_player");

    exeTestRunner brokerExe (std::string (HELICS_INSTALL_LOC), std::string (HELICS_BUILD_LOC) + "/apps/",
                             "helics_broker");

    BOOST_REQUIRE (playerExe.isActive ());
    BOOST_REQUIRE (brokerExe.isActive ());
    auto res = brokerExe.runAsync ("2 --type=zmq --name=zmq_broker");
    std::string exampleFile = std::string (TEST_DIR) + "/test_files/" + file;
    auto res2 = playerExe.runCaptureOutputAsync ("--name=player --core=zmq " + exampleFile);

    helics::FederateInfo fi ("fed");
    fi.coreType = helics::core_type::ZMQ;
    fi.coreInitString = "1";

    helics::ValueFederate vfed (fi);
    helics::Subscription sub1 (&vfed, "pub1");
    helics::Subscription sub2 (&vfed, "pub2");
    vfed.enterExecutionState ();
    auto val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.3);

    auto retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 1.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.5);
    val = sub2.getValue<double> ();
    BOOST_CHECK_CLOSE (val, 0.4, 0.000001);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 2.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.7);
    val = sub2.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.6);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 3.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.8);
    val = sub2.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.9);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 5.0);
    vfed.finalize ();
    auto out2 = res2.get ();
    res.get ();
    // out = 0;
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}
#endif

BOOST_AUTO_TEST_CASE (simple_player_testjson)
{
    helics::FederateInfo fi ("player1");
    fi.coreType = helics::core_type::TEST;
    fi.coreName = "core7";
    fi.coreInitString = "2";
    helics::apps::Player play1 (fi);
    fi.name = "block1";
    play1.loadFile (std::string (TEST_DIR) + "/test_files/example6.json");

    helics::ValueFederate vfed (fi);
    helics::Subscription sub1 (&vfed, "pub1");
    helics::Subscription sub2 (&vfed, "pub2");
    auto fut = std::async (std::launch::async, [&play1]() { play1.run (); });
    vfed.enterExecutionState ();

    auto retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 1.0);
    auto val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.5);
    val = sub2.getValue<double> ();
    BOOST_CHECK_CLOSE (val, 0.4, 0.000001);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 2.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.7);
    val = sub2.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.6);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 3.0);
    val = sub1.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.8);
    val = sub2.getValue<double> ();
    BOOST_CHECK_EQUAL (val, 0.9);

    retTime = vfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 5.0);
    vfed.finalize ();
    fut.get ();
}

BOOST_AUTO_TEST_CASE (player_test_message)
{
    helics::FederateInfo fi ("player1");
    fi.coreType = helics::core_type::TEST;
    fi.coreName = "core8";
    fi.coreInitString = "2";
    helics::apps::Player play1 (fi);
    fi.name = "block1";

    helics::MessageFederate mfed (fi);
    helics::Endpoint e1 (helics::GLOBAL, &mfed, "dest");

    play1.addMessage (1.0, "src", "dest", "this is a message");
    auto fut = std::async (std::launch::async, [&play1]() { play1.run (); });
    mfed.enterExecutionState ();

    auto retTime = mfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 1.0);
    auto mess = e1.getMessage ();
    BOOST_CHECK (mess);
    if (mess)
    {
        BOOST_CHECK_EQUAL (mess->source, "src");
        BOOST_CHECK_EQUAL (mess->dest, "dest");
        BOOST_CHECK_EQUAL (mess->data.to_string (), "this is a message");
    }

    mfed.finalize ();
    fut.get ();
}

BOOST_AUTO_TEST_CASE (player_test_message2)
{
    helics::FederateInfo fi ("player1");
    fi.coreType = helics::core_type::TEST;
    fi.coreName = "core9";
    fi.coreInitString = "2";
    helics::apps::Player play1 (fi);
    fi.name = "block1";

    helics::MessageFederate mfed (fi);
    helics::Endpoint e1 (helics::GLOBAL, &mfed, "dest");

    play1.addMessage (1.0, "src", "dest", "this is a test message");
    play1.addMessage (2.0, "src", "dest", "this is test message2");

    play1.addMessage (3.0, "src", "dest", "this is message 3");
    auto fut = std::async (std::launch::async, [&play1]() { play1.run (); });
    mfed.enterExecutionState ();

    auto retTime = mfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 1.0);
    auto mess = e1.getMessage ();
    BOOST_CHECK (mess);
    if (mess)
    {
        BOOST_CHECK_EQUAL (mess->source, "src");
        BOOST_CHECK_EQUAL (mess->dest, "dest");
        BOOST_CHECK_EQUAL (mess->data.to_string (), "this is a test message");
    }

    retTime = mfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 2.0);
    mess = e1.getMessage ();
    BOOST_CHECK (mess);
    if (mess)
    {
        BOOST_CHECK_EQUAL (mess->source, "src");
        BOOST_CHECK_EQUAL (mess->dest, "dest");
        BOOST_CHECK_EQUAL (mess->data.to_string (), "this is test message2");
    }

    retTime = mfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 3.0);
    mess = e1.getMessage ();
    BOOST_CHECK (mess);
    if (mess)
    {
        BOOST_CHECK_EQUAL (mess->source, "src");
        BOOST_CHECK_EQUAL (mess->dest, "dest");
        BOOST_CHECK_EQUAL (mess->data.to_string (), "this is message 3");
    }
    mfed.finalize ();
    fut.get ();
}

BOOST_AUTO_TEST_CASE (player_test_message3)
{
    helics::FederateInfo fi ("player1");
    fi.coreType = helics::core_type::TEST;
    fi.coreName = "core10";
    fi.coreInitString = "2";
    helics::apps::Player play1 (fi);
    fi.name = "block1";

    helics::MessageFederate mfed (fi);
    helics::Endpoint e1 (helics::GLOBAL, &mfed, "dest");

    play1.addMessage (1.0, "src", "dest", "this is a test message");
    play1.addMessage (1.0, 2.0, "src", "dest", "this is test message2");

    play1.addMessage (2.0, 3.0, "src", "dest", "this is message 3");
    // mfed.getCorePointer()->setLoggingLevel(helics::invalid_fed_id, 5);
    auto fut = std::async (std::launch::async, [&play1]() { play1.run (); });
    mfed.enterExecutionState ();

    auto retTime = mfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 1.0);
    auto mess = e1.getMessage ();
    BOOST_CHECK (mess);
    if (mess)
    {
        BOOST_CHECK_EQUAL (mess->source, "src");
        BOOST_CHECK_EQUAL (mess->dest, "dest");
        BOOST_CHECK_EQUAL (mess->data.to_string (), "this is a test message");
    }

    retTime = mfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 2.0);
    mess = e1.getMessage ();
    BOOST_CHECK (mess);
    if (mess)
    {
        BOOST_CHECK_EQUAL (mess->source, "src");
        BOOST_CHECK_EQUAL (mess->dest, "dest");
        BOOST_CHECK_EQUAL (mess->data.to_string (), "this is test message2");
    }

    retTime = mfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 3.0);
    mess = e1.getMessage ();
    BOOST_CHECK (mess);
    if (mess)
    {
        BOOST_CHECK_EQUAL (mess->source, "src");
        BOOST_CHECK_EQUAL (mess->dest, "dest");
        BOOST_CHECK_EQUAL (mess->data.to_string (), "this is message 3");
    }
    mfed.finalize ();
    fut.get ();
}

const std::vector<std::string> simple_message_files{"example_message1.player", "example_message2.player",
                                                    "example_message3.json"};

BOOST_DATA_TEST_CASE (simple_message_player_test_files, boost::unit_test::data::make (simple_message_files), file)
{
    static char indx = 'a';
    helics::FederateInfo fi ("player1");
    fi.coreType = helics::core_type::TEST;
    fi.coreName = "core11";
    fi.coreName.push_back (indx++);
    fi.coreInitString = "2";
    helics::apps::Player play1 (fi);
    fi.name = "block1";

    helics::MessageFederate mfed (fi);
    helics::Endpoint e1 (helics::GLOBAL, &mfed, "dest");
    play1.loadFile (std::string (TEST_DIR) + "/test_files/" + file);
    auto fut = std::async (std::launch::async, [&play1]() { play1.run (); });
    mfed.enterExecutionState ();

    auto retTime = mfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 1.0);
    auto mess = e1.getMessage ();
    BOOST_CHECK (mess);
    if (mess)
    {
        BOOST_CHECK_EQUAL (mess->source, "src");
        BOOST_CHECK_EQUAL (mess->dest, "dest");
        BOOST_CHECK_EQUAL (mess->data.to_string (), "this is a test message");
    }

    retTime = mfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 2.0);
    mess = e1.getMessage ();
    BOOST_CHECK (mess);
    if (mess)
    {
        BOOST_CHECK_EQUAL (mess->source, "src");
        BOOST_CHECK_EQUAL (mess->dest, "dest");
        BOOST_CHECK_EQUAL (mess->data.to_string (), "this is test message2");
    }

    retTime = mfed.requestTime (5);
    BOOST_CHECK_EQUAL (retTime, 3.0);
    mess = e1.getMessage ();
    BOOST_CHECK (mess);
    if (mess)
    {
        BOOST_CHECK_EQUAL (mess->source, "src");
        BOOST_CHECK_EQUAL (mess->dest, "dest");
        BOOST_CHECK_EQUAL (mess->data.to_string (), "this is message 3");
    }
    mfed.finalize ();
    fut.get ();
}

BOOST_AUTO_TEST_CASE(player_test_help)
{
    StringToCmdLine cmdArg("--version --quiet"); 
    helics::apps::Player play1(cmdArg.getArgCount(),cmdArg.getArgV());

    BOOST_CHECK(!play1.isActive());

    StringToCmdLine cmdArg2("-? --quiet");
    helics::apps::Player play2(cmdArg2.getArgCount(), cmdArg2.getArgV());

    BOOST_CHECK(!play2.isActive());
}
#ifndef DISABLE_SYSTEM_CALL_TESTS
/*
BOOST_AUTO_TEST_CASE (simple_player_test)
{
    static exeTestRunner playerExe (HELICS_BIN_LOC "apps/", "helics_player");

    static exeTestRunner brokerExe (HELICS_BIN_LOC "apps/", "helics_broker");

    auto res = brokerExe.runAsync ("1 --type=ipc --name=ipc_broker");
    std::string exampleFile = std::string (TEST_DIR) + "/test_files/example1.Player";
    auto res2 = playerExe.runCaptureOutputAsync ("--name=Player --broker=ipc_broker --core=ipc " + exampleFile);

    auto val = res2.get ();
    auto val2 = res.get ();
    BOOST_CHECK_EQUAL (val2, 0);
    std::string compareString = "read file 24 points for 1 tags";
    BOOST_CHECK (val.compare (0, compareString.size (), compareString) == 0);
}
*/
#endif

BOOST_AUTO_TEST_SUITE_END ()
