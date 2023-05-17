cmake_minimum_required(VERSION 3.22)

project(CDataPyarrowExample VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Arrow CONFIG REQUIRED)
find_package(Python COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(MyModule module.cc)
target_link_libraries(
  MyModule
  PRIVATE
  # It can be slighlty more convenient to use Arrow::arrow_static but that target
  # is not included with the conda install so we are using Arrow::arrow_shared for
  # portability.
  Arrow::arrow_shared
)
