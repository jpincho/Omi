# CMakeList.txt : CMake project for Omi, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("Omi")

# Add source to this project's executable.
add_executable (Omi "Omi.cpp" "Omi.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Omi PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
