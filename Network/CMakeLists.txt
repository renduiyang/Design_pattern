cmake_minimum_required(VERSION 3.20)

project(network)

set(Client ON)
set(Server ON)
set(Test 1)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
message("output path is ${CMAKE_CURRENT_SOURCE_DIR}/bin")


if (Client)
    add_subdirectory(client)
endif ()

if (Server)
    add_subdirectory(server)
endif ()

if (Test)
    add_subdirectory(test)
endif ()

