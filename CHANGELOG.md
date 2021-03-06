# Changelog
All notable changes to this project after the 1.0.0 release will be documented in this file

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/).  
This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.3.0] - 2018-07-31
### Changed
 - some CMake options have been removed (BUILD_BROKER)
 - major changes to the build of the CTest testing Framework

### Fixed
 - potential segmentation fault in C shared library when calling free with invalid object.
 - autobuild recognizes build configuration

### Added
 - working octave interface for Linux
 - some additional tests for the shared library
 - TOML readers for interface description in Federates
 - interactive command line for helics_broker
 - a few new queries on brokers see [Queries](docs/user_guide/Queries.md)
 - CPACK can now build a dmg files
 - Players can have multiline comments in input file and omit the tag for repeated messages
 - marker option on player, recorder, tracer to print time advancement message

### Changed
 - added better code for allowing static runtime builds
 - use the cmake version numbers instead of independent variables
 - Environment variables are recognized in CMAKE find options
 - split API tests from system wide tests
 - added options on MSVC to build with embedded system libraries and embedded debug info.

## [1.2.1] - 2018-06-30
### Fixed
 - bug in the conversion of named points from strings
 - MATLAB helicsSubscriptionGetVector function was producing a segmentation fault, now this is fixed
 - performance issue in the delay buffers of federateState
 - findMPI for MPI libraries with multiple libraries
 - federates will now error on missing required publications

### Added
 - first cut of MATLAB interface tests
 - some additional Java test cases
 - Python test cases for named point and bool tests
 - MATLAB helper scripts for loading the library
 - String length function for subscriptions

### Changed
 - conversion of doubles into the internal time base now rounds to the nearest ns instead of truncating
 - unify cmake scripts to use lower case commands

## [1.2.0] - 2018-06-18
### Fixed
 - issue with various filter types and random drop filters
 - a few minor issues with C API (helicsFederateSetMaxIterations)
 - potential threading issues when updating the logger on an executing federate
 - federates will now propagate errors properly for duplicate publications and endpoints
 - federates will now error on missing required publications

### Changed
 - implement use of FMT library instead of boost::format
 - improved python installation scripts
 - general threading and refactoring of the core
 - automatic name generation no longer uses random uuid's, but a shorter 20 fully character random string

### Added
 - queryCoreExecute, queryBrokerExecute to the C API to allow queries to be executed directly on brokers and cores
 - C++ API changes to for Brokers and Core to allow queries
 - Get StringLength function to the C and interface API
 - new queries (federate_map, and dependency_graph(partial))
 - additional filter tests and query tests
 - realtime mode for HELICS specified by activating the realtime flag, and specifying rt_lag and rt_lead  the federate will then delay grant or force_grant based on computer clock to match wall time.

## [1.1.1] - 2018-05-25
### Added
 - BrokerApp as a slightly more convenient runner to Brokers
 - getXXSize functions directly in the Subscription object instead of a roundabout call in the C api
 - more complete error catching for the C library
 - added helics-config executable for getting paths and links and used flags
 - added a broker app that can start up a broker easily

### Changed
 - upgrade autobuild ZMQ version to 4.2.5 and change CMake scripts to use zmq target
 - updated HELICSConfig.cmake install file to link properly to external libraries and find them if necessary, also included some find functions.  The find_package(HELICS) should work properly now
 - changed boost inclusion to use targets instead of files directly
 - changed MPI inclusion to work better on windows and use targets instead of direct links
 - update cereal library with latest bug fixes
 - update jsoncpp with latest version
 - update cppzmq with the latest version
 - moved helics_broker executable code to the apps repository
 - the CXX shared library can now be built alongside the C shared library and can be built on Windows.

### Fixed
 - compilation issue with Xcode 8.0
 - inconsistent numerical conversion from vectors to doubles in subscriptions

### Removed
 - installation of HELICSImport.cmake  this is now redundant with updated HELICSConfig.cmake

## [1.1.0] - 2018-05-09
### Added
 - namedpoint functions in the C++ for publications and subscriptions, and corresponding functions in the C interface and language API's
 - Boolean publication and subscription for C++ interface, and corresponding functions in the C interface and language API's
 - new options for brokers, --local, --ipv4, --ipv6, --all,  are shortcuts for specifying external network interfaces
 - additional documentation, CONTRIBUTORS, ROADMAP, CONTRIBUTIONS, and some other other documentation improvements

### Changed
 - the default interface configuration for federates and brokers.  The --interface option is less important as interfaces should mostly get automatically determined by the broker address
 - minor configuration changes to CMAKE configuration to be more conforming with modern CMAKE best practices
 - cleaned up header installation for app directory
 - shared library construction now uses some headers generated by CMAKE

### Fixed
 - better error checking in the C interface
 - fixes for occasionally failing tests

## [1.0.3] - 2018-04-28
### Fixed
 - Fix bug preventing federates from terminating if its dependencies are disconnected and using purely interrupt driven timing, such as a recorder

## [1.0.2] - 2018-04-27
### Fixed
 - Bug not allowing command line parameters separate from the command if a positional argument was in usage
 - Fixed Bug for federate not allowing changes in period or minTimeDelay after entry to execution mode
 - added python2 interface option (this will be available but not fully capable going forward)
 - A few more race conditions fixed from clang thread-sanitizer

## [1.0.1] - 2018-04-22
### Fixed
 - Allow Boost 1.67 usage
 - allow building with AUTOBUILD for ZeroMQ on Linux
 - Clang tidy and static analyzer fixes
 - fix some potential race conditions spotted by clang thread-sanitizer
 - Fix some documentation to better match recent updates
