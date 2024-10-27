set(srcPath src)

include_directories(include)

set(ZeroMQ_DIR D:/workspace/environment/libzmq/CMake)
#
find_package(ZeroMQ REQUIRED)

file(GLOB_RECURSE mySourceFile CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

file(GLOB_RECURSE myHSourceFile CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)

foreach (source IN ITEMS ${mySourceFile})
    get_filename_component(name ${source} NAME_WLE)
    add_executable(${name} ${source} ${myHSourceFile})
    message("name is ${name}")
    target_link_libraries(${name} ws2_32 libzmq-static)
endforeach ()


