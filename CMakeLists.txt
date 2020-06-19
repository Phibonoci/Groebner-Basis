cmake_minimum_required(VERSION 3.15)
project(Groebner-Basis)

set(CMAKE_CXX_STANDARD 20)

add_executable(Groebner-Basis main.cpp rational.h test.cpp test.h overflow_detector.h monomial.h polynomial.h concepts.h order.h algorithms.h cyclic.h modular.h)

set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Werror -fconcepts -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )