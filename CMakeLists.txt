cmake_minimum_required(VERSION 2.8.12)
project (leros-sim)
set (CMAKE_CXX_STANDARD 14)

option(LEROS64 "Leros 64-bit simulator" OFF)


# ELFIO
file(GLOB ELFIO_H external/elfio/*.hpp)

file(GLOB CXXOPTS_H external/cxxopts/*.hpp)

file(GLOB RIPES_H external/ripes/*.h)


add_executable(leros-sim leros-sim.cpp leros-sim.h ${ELFIO_H} ${CXXOPTS_H} ${RIPES_H})

include_directories(leros-sim public "external")

if(LEROS64)
    target_compile_definitions(leros-sim PRIVATE -DLEROS64)
endif()
