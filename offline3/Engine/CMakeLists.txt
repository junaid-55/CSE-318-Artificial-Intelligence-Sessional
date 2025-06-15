cmake_minimum_required(VERSION 3.14)
project(Chain_reaction)

include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        v2.11.1
)
FetchContent_MakeAvailable(pybind11)

# Add source files
pybind11_add_module(Chain_reaction 
    bindings.cpp
)

# Include directories
target_include_directories(Chain_reaction PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})