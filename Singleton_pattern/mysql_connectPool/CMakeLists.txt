message("this is a mysql connect Pool demo")
include_directories(include)
link_directories(lib)

set(goole_Test ON)
set(goole_Test_Path D:/workspace/environment/gooletest_static)
set(mysql_Path D:/workspace/environment/mysql5.7)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(LIB_Name mysql_connect)


file(GLOB_RECURSE mySourceFile CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

file(GLOB_RECURSE mytestFile CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp)

file(GLOB_RECURSE myHSourceFile CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)

message("file is ${mySourceFile}")

include_directories(${mysql_Path}/include)
link_directories(${mysql_Path}/lib)

if (goole_Test)
    message("goole_test on")
    include_directories(${goole_Test_Path}/include)
    link_directories(${goole_Test_Path}/lib)
    foreach (source IN ITEMS ${mytestFile})
        get_filename_component(name ${source} NAME_WLE)
        add_executable(${name} ${source} ${myHSourceFile})
        message("name is ${name}")
        target_link_libraries(${name} Qt5::Widgets Qt5::Sql Qt5::Core gmock gtest gtest_main mysql)
        enable_testing()
        add_test(NAME ${name} COMMAND ${name})
    endforeach ()
    #    set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    #    add_library(${LIB_Name} STATIC ${myHppFile})
else ()
    message("no goole_test")
    foreach (source IN ITEMS ${mySourceFile})
        get_filename_component(name ${source} NAME_WLE)
        add_executable(${name} ${source} ${myHSourceFile})
        message("name is ${name}")
        target_link_libraries(${name} Qt5::Widgets Qt5::Sql Qt5::Core mysql)
    endforeach ()
endif ()
