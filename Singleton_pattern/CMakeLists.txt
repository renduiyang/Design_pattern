cmake_minimum_required(VERSION 3.20)

project(factor_designer)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(src_path src)
find_package(Qt5 COMPONENTS Widgets Core Sql REQUIRED)

set(TestDemo ON)

# GLOB_RECURSE 和 GLOB 的主要区别在于搜索文件的方式：
# GLOB 只会搜索当前目录下 不会去搜寻子目录下文件
# GLOB_RECURSE 则会递归搜寻子目录下文件
# 如果要排除文件 先全部查找一遍 然后查找要排除的文件  最后通过 list(REMOVE_ITEM) 命令从原始列表中移除这些文件。

if (NOT TestDemo)
    file(GLOB_RECURSE mySourceFile CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/${src_path}/*.cpp)

    message("file is ${mySourceFile}, source dir is ${CMAKE_SOURCE_DIR} ")

    foreach (source IN ITEMS ${mySourceFile})
        get_filename_component(name ${source} NAME_WLE)
        add_executable(${name} ${source})
        target_link_libraries(${name} Qt5::Widgets Qt5::Sql Qt5::Core)
    endforeach ()

else ()
    add_subdirectory(mysql_connectPool)
endif ()