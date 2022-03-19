cmake_minimum_required(VERSION 3.21)
project(mc859)

set(CMAKE_CXX_STANDARD 14)

# Headers directories
include_directories("include")
include_directories($ENV{GUROBI_HOME}/include)

# Gurobi lib directory
link_directories($ENV{GUROBI_HOME}/lib)

add_executable(mc859 src/main.cpp)
target_link_libraries(mc859 m gurobi_c++ gurobi95)
