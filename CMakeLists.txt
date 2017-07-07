cmake_minimum_required (VERSION 2.6)
project (ArtnetToSerial)
project (demo)

set (CMAKE_CXX_STANDARD 11)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Boost 1.50 COMPONENTS filesystem log REQUIRED )

configure_file (
        "config.h.in"
        "config.h"
)
add_definitions(-DBOOST_LOG_DYN_LINK)

include_directories ("${PROJECT_SOURCE_DIR}/lib/imgui")
add_subdirectory (lib)

add_executable(ArtnetToSerial imgui_impl_glfw.cpp artnetToSerial.cpp SerialInterface.cpp gui/SerialWindow.cpp LoggingUtilities.cpp gui/LogWindow.cpp)
target_link_libraries (ArtnetToSerial ImGui ImGuiDemo glfw GL)
target_link_libraries (ArtnetToSerial glfw GL)
target_link_libraries (ArtnetToSerial ${Boost_LIBRARIES} pthread)

add_executable(demo demo.cpp SerialInterface.cpp)
target_link_libraries ( demo ${Boost_LIBRARIES} pthread)

set(CMAKE_VERBOSE_MAKEFILE ON)
#set(CMAKE_CXX_FLAGS "-O3")
