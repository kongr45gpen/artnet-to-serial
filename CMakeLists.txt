cmake_minimum_required (VERSION 2.6)
cmake_policy(VERSION 3.13)
project (ArtnetToSerial)
project (demo)

set (CMAKE_CXX_STANDARD 11)

set(OpenGL_GL_PREFERENCE "GLVND")

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Boost 1.67 COMPONENTS system filesystem log thread REQUIRED)

# Includes for compiler checks
include(CheckTypeSize)
set(CMAKE_REQUIRED_LIBRARIES ${Boost_LIBRARIES} pthread)
#set(CMAKE_REQUIRED_DEFINITIONS "-DBOOST_ASIO_ENABLE_OLD_SERVICES")
set(CMAKE_EXTRA_INCLUDE_FILES "boost/asio/basic_serial_port.hpp")

# Check if boost::asio::basic_serial_port exists
check_type_size(boost::asio::basic_serial_port<> BOOST_BASIC_SERIAL_PORT LANGUAGE CXX)

configure_file (
        "config.h.in"
        "config.h"
)
add_definitions(-DBOOST_LOG_DYN_LINK)

include_directories ("${PROJECT_SOURCE_DIR}/lib/imgui" "${PROJECT_SOURCE_DIR}/lib/imgui/examples" "${PROJECT_SOURCE_DIR}/lib/imgui/examples/libs/gl3w")
add_subdirectory (lib)

#add_executable(demo demo.cpp SerialInterface.cpp)
#target_link_libraries ( demo ${Boost_LIBRARIES} pthread)

add_executable(ArtnetToSerial
        artnetToSerial.cpp
        SerialInterface.cpp
        gui/SerialWindow.cpp
        LoggingUtilities.cpp
        gui/LogWindow.cpp
        gui/ArtnetWindow.cpp
        ArtnetThread.cpp
        DMXBucket.cpp
        gui/DMXWindow.cpp
        gui/ActivityLED.cpp
        DataStatistics.cpp
        SerialThread.cpp)
target_link_libraries (ArtnetToSerial ImGui ImGuiDemo)
target_link_libraries (ArtnetToSerial ImGuiOpenGL3 gl3w glfw GL ${CMAKE_DL_LIBS})
target_link_libraries (ArtnetToSerial ${Boost_LIBRARIES} pthread)

set(CMAKE_VERBOSE_MAKEFILE ON)
#set(CMAKE_CXX_FLAGS "-O3")
#set(CMAKE_CXX_FLAGS "-g")
