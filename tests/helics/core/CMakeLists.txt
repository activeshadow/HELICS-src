
add_executable(core-factory-test core-factory-test.cc)
target_link_libraries(core-factory-test helics)

add_executable(node-core-test node-core-test.cc)
target_link_libraries(node-core-test helics)

add_test(NAME core-factory-test COMMAND core-factory-test)

#-----------------------------------------------------------------------------
# Core tests using Boost
#-----------------------------------------------------------------------------

set(core_test_headers
)

set(core_test_sources
core-tests.cpp
zeromqcore-tests.cpp
testcore-tests.cpp)

add_executable(core-tests ${core_test_sources} ${core_test_headers})
target_link_libraries(core-tests helics)

link_directories(${external_link_directories})

include_directories(${Boost_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/ThirdParty)

target_link_libraries(core-tests ${external_library_list})

add_test(NAME core-tests COMMAND core-tests)

#-----------------------------------------------------------------------------
# Setup CTEST environment
#-----------------------------------------------------------------------------
include(CTest)


foreach(keyfile IN LISTS KEY_LIBRARY_FILES)
add_custom_command(TARGET core-tests POST_BUILD        # Adds a post-build event to core tests
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${keyfile}"      # <--this is in-file
        "$<TARGET_FILE_DIR:core-tests>/")                 # <--this is out-file path
endforeach(keyfile)