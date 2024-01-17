# CMakeList.txt : CMake project for Assignment, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("Assignment")

# Add source to this project's executable.
add_executable (Assignment "Assignment.cpp" "Assignment.h")

target_include_directories(Assignment PUBLIC "./include") 
target_link_directories(Assignment PUBLIC "./lib")
target_link_libraries(Assignment sfml-main-d sfml-graphics-d sfml-system-d sfml-window-d tgui-d)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Assignment PROPERTY CXX_STANDARD 20)
endif()

