```cmake

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Windows Detected!")
    # 如果是 Windows，设置 Boost 根目录
    set(Boost_ROOT "D:/CppBoost/boost_1_84_0/build_mingw")
    message(STATUS "set Boost_ROOT to ${Boost_ROOT}")
endif()
set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
# required components
find_package(Boost REQUIRED COMPONENTS system filesystem thread chrono)
set(Boost_NO_SYSTEM_PATHS ON) # 只搜索上语句设置的搜索路径
if(Boost_FOUND)
    message(${Boost_INCLUDE_DIRS})
    message(${Boost_LIBRARIES})
endif()

#---------------------------

message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB_RECURSE mySourceFile CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
message(STATUS ${mySourceFile})
#set(CMAKE_CXX_FLAGS -lboost_system)
foreach (source IN ITEMS ${mySourceFile})
    message(STATUS ${source})
    get_filename_component(name "${source}" NAME_WLE)
    message(STATUS ${name})
    add_executable(${name} ${source})
    target_link_libraries(${name} PUBLIC ${Boost_LIBRARIES} ws2_32)
endforeach ()

```



```cmake
# cmake选择性的引入boost动态库或者静态库
option(USE_STATIC_BOOST "Build with static BOOST libraries instead of dynamic" NO)
if(USE_STATIC_BOOST)
   unset(Boost_LIBRARIES)
   message(WARNING "Linking against boost static libraries")
   set(Boost_USE_STATIC_LIBS ON)
   set(Boost_USE_MULTITHREADED ON)
   find_package(Boost REQUIRED COMPONENTS thread program_options system)
else(USE_STATIC_BOOST)
   unset(Boost_LIBRARIES)
   message(WARNING "Linking against boost dynamic libraries")
   set(Boost_USE_STATIC_LIBS OFF)
   set(Boost_USE_MULTITHREADED ON)
   find_package(Boost REQUIRED COMPONENTS thread program_options system)
endif(USE_STATIC_BOOST)
```

```c++
#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

void test1() {
    using namespace std;
    cout << "Enter your weight: ";
    float weight;
    cin >> weight;
    string gain = "A 10% increase raises ";
    string wt = boost::lexical_cast<string>(weight);
    gain = gain + wt + " to "; // string operator()
    weight = 1.1 * weight;
    gain = gain + boost::lexical_cast<string>(weight) + ".";
    cout << gain << endl;
    system("pause");
}

void test2() {
    namespace boostfs = boost::filesystem;
    std::string mystr = "D:/C++_project/Design_pattern/CRTP/learn/learn_main.cpp";
    boostfs::path p(mystr);

    if (boostfs::exists(p)) {
        std::cout << "File " << p << " exists." << std::endl;
    } else {
        std::cout << "File " << p << " does not exist." << '\n';
    }
}

int main() {
    test2();
    return 0;
}

```