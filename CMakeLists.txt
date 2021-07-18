#
# MIT License
#
# Copyright (c) 2021 Olivier Le Doeuff
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
## CMAKE INPUT
#
#   - RECYCLER_TARGET : Library target name. Default : "Recycler"
#   - RECYCLER_PROJECT : Project name. Default : "Recycler"
#   - RECYCLER_BUILD_SHARED : Build shared library [ON OFF]. Default: OFF.
#   - RECYCLER_BUILD_STATIC : Build static library [ON OFF]. Default: ON.
#
#   - RECYCLER_ENABLE_TESTS : Build Recycler Test executable [ON OFF]. Default: OFF.
#
#   - GTEST_REPOSITORY : Repository of gtest, can be a local url [URL]. Default  https://github.com/google/googletest.git.
#   - GTEST_TAG : Version of gtest [master release-1.8.0]. Default : release-1.8.0.
#
## CMAKE OUTPUT
#
#   - RECYCLER_TARGET : Output target to link to. Default: Recycler
#   - RECYCLER_VERSION : Version of Recycler library
#

# ┌──────────────────────────────────────────────────────────────────┐
# │                       CMAKE PROPERTIES                           │
# └──────────────────────────────────────────────────────────────────┘

cmake_minimum_required(VERSION 3.14.0 FATAL_ERROR)

# ┌──────────────────────────────────────────────────────────────────┐
# │                       PROJECT SETTINGS                           │
# └──────────────────────────────────────────────────────────────────┘

# OPTIONS

set(RECYCLER_VERSION_MAJOR 1)
set(RECYCLER_VERSION_MINOR 3)
set(RECYCLER_VERSION_PATCH 2)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
  EXECUTE_PROCESS(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE RECYCLER_VERSION_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()
if(NOT RECYCLER_VERSION_TAG)
  set(RECYCLER_VERSION_TAG 00000000)
endif()
set(RECYCLER_VERSION_TAG_HEX 0x${RECYCLER_VERSION_TAG})
set(RECYCLER_VERSION ${RECYCLER_VERSION_MAJOR}.${RECYCLER_VERSION_MINOR}.${RECYCLER_VERSION_PATCH} CACHE STRING "" FORCE)

# General
set(RECYCLER_PROJECT "Recycler" CACHE STRING "Project Name")
set(RECYCLER_TARGET ${RECYCLER_PROJECT} CACHE STRING "Recycler library name")
set(RECYCLER_VERSION ${RECYCLER_VERSION_MAJOR}.${RECYCLER_VERSION_MINOR}.${RECYCLER_VERSION_PATCH} CACHE STRING "Recycler current version, this is only decorative and will not configure any files" FORCE)

set(RECYCLER_FOLDER_PREFIX ${RECYCLER_PROJECT} CACHE STRING "Prefix folder for all Recycler generated targets in generated project (only decorative)")

# Tests
set(RECYCLER_ENABLE_TESTS OFF CACHE BOOL "Create or not a target for test (compatible with CTests)")
set(RECYCLER_TESTS_PREFIX ${RECYCLER_PROJECT} CACHE STRING "Prefix for all Recycler tests")

# Install
set(RECYCLER_ENABLE_INSTALL OFF CACHE BOOL "Create or not a target for install")
set(RECYCLER_INSTALL_PREFIX ${RECYCLER_PROJECT} CACHE STRING "Prefix for all Recycler headers in the install folder")

# CREATE PROJECT

project(${RECYCLER_PROJECT} VERSION ${RECYCLER_VERSION} LANGUAGES CXX)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 14)

# LOG OPTIONS

message(STATUS "---------------- RECYCLER OPTIONS. ----------------")

#General
message(STATUS "RECYCLER_PROJECT            : " ${RECYCLER_PROJECT})
message(STATUS "RECYCLER_TARGET             : " ${RECYCLER_TARGET})
message(STATUS "RECYCLER_VERSION            : " ${RECYCLER_VERSION})
message(STATUS "RECYCLER_VERSION_TAG        : " ${RECYCLER_VERSION_TAG})
message(STATUS "RECYCLER_FOLDER_PREFIX      : " ${RECYCLER_FOLDER_PREFIX})

# Tests
message(STATUS "RECYCLER_ENABLE_TESTS       : " ${RECYCLER_ENABLE_TESTS})
if(RECYCLER_ENABLE_TESTS)
  message(STATUS "RECYCLER_TESTS_PREFIX       : " ${RECYCLER_TESTS_PREFIX})
endif(RECYCLER_ENABLE_TESTS)

# Install
message(STATUS "RECYCLER_ENABLE_INSTALL     : " ${RECYCLER_ENABLE_INSTALL})
if(RECYCLER_ENABLE_INSTALL)
  message(STATUS "RECYCLER_INSTALL_PREFIX     : " ${RECYCLER_INSTALL_PREFIX})
endif(RECYCLER_ENABLE_INSTALL)

message(STATUS "---------------- DONE WITH OPTIONS. -----------------")

# ┌──────────────────────────────────────────────────────────────────┐
# │                       SOURCES                                    │
# └──────────────────────────────────────────────────────────────────┘

set(RECYCLER_INCS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(RECYCLER_PRIV_INCS_DIR ${RECYCLER_INCS_DIR}/Recycler)

set(RECYCLER_SRCS
  ${RECYCLER_PRIV_INCS_DIR}/Recycler.hpp
  ${RECYCLER_PRIV_INCS_DIR}/Circular.hpp
  ${RECYCLER_PRIV_INCS_DIR}/Buffer.hpp
  )
source_group("Recycler" FILES ${RECYCLER_SRCS})

# ┌──────────────────────────────────────────────────────────────────┐
# │                       TARGET                                     │
# └──────────────────────────────────────────────────────────────────┘

# Recycler Target
add_library(${RECYCLER_TARGET} INTERFACE)
add_library(${RECYCLER_TARGET}::${RECYCLER_TARGET} ALIAS ${RECYCLER_TARGET})
target_sources(${RECYCLER_TARGET} INTERFACE "${RECYCLER_SRCS}")

# Set include directory
target_include_directories(${RECYCLER_TARGET} INTERFACE $<BUILD_INTERFACE:${RECYCLER_INCS_DIR}> $<INSTALL_INTERFACE:${RECYCLER_INSTALL_PREFIX}>)

# Version
target_compile_definitions(${RECYCLER_TARGET} INTERFACE -DRECYCLER_VERSION_MAJOR=${RECYCLER_VERSION_MAJOR})
target_compile_definitions(${RECYCLER_TARGET} INTERFACE -DRECYCLER_VERSION_MINOR=${RECYCLER_VERSION_MINOR})
target_compile_definitions(${RECYCLER_TARGET} INTERFACE -DRECYCLER_VERSION_PATCH=${RECYCLER_VERSION_PATCH})
target_compile_definitions(${RECYCLER_TARGET} INTERFACE -DRECYCLER_VERSION_TAG=${RECYCLER_VERSION_TAG})
target_compile_definitions(${RECYCLER_TARGET} INTERFACE -DRECYCLER_VERSION_TAG_HEX=${RECYCLER_VERSION_TAG_HEX})

# ┌──────────────────────────────────────────────────────────────────┐
# │                           TESTS                                  │
# └──────────────────────────────────────────────────────────────────┘

if(RECYCLER_ENABLE_TESTS)
  include(cmake/FetchGTest.cmake)
  enable_testing()
  add_subdirectory(tests)
endif()

# ┌──────────────────────────────────────────────────────────────────┐
# │                         INSTALL                                  │
# └──────────────────────────────────────────────────────────────────┘

if(RECYCLER_ENABLE_INSTALL)
  include(CInstallLists.txt)
endif()
