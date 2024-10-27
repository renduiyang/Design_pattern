file(GLOB_RECURSE mySourceFile CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)


foreach (source IN ITEMS ${mySourceFile})
    get_filename_component(name ${source} NAME_WLE)
    add_executable(${name} ${source})
    message("name is ${name}")
    target_link_libraries(${name})
endforeach ()