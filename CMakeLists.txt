#
#   SharedCache CMake script by Olivier Le Doeuff
#
## CMAKE INPUT
#
#   - SHAREDCACHE_TARGET : Library target name. Default : "SharedCache"
#   - SHAREDCACHE_PROJECT : Project name. Default : "SharedCache"
#   - SHAREDCACHE_BUILD_SHARED : Build shared library [ON OFF]. Default: OFF.
#   - SHAREDCACHE_BUILD_STATIC : Build static library [ON OFF]. Default: ON.
#   - SHAREDCACHE_USE_NAMESPACE : Should the library be compiled with a namespace. Default: ON
#   - SHAREDCACHE_NAMESPACE : Namespace of the library is SHAREDCACHE_USE_NAMESPACE is ON. Default : SharedCache
#
#   - LIBENDIAN_REPOSITORY : Repository of LibEndian. Default : "https://github.com/OlivierLDff/LibEndian.git".
#   - LIBENDIAN_TAG : Tag of LibEndian. Default : "v1.0.2".
#
#   - QQMLMODEL_REPOSITORY : Repository of QQmlModel. Default : "https://github.com/OlivierLDff/QQmlModel.git".
#   - QQMLMODEL_TAG : Tag of QQmlModel. Default : "v1.0.4".
#
#   - QSUPERMACROS_REPOSITORY : Repository of QSuperMacros. Default : "https://github.com/OlivierLDff/QSuperMacros".
#   - QSUPERMACROS_TAG : Tag of QSuperMacros. Default : "v1.1.1".
#
#   - SHAREDCACHE_BUILD_DOC : Build the SharedCache Doc
#   - DOXYGEN_BT_REPOSITORY : Repository of DoxygenBt. Default: "https://github.com/OlivierLDff/DoxygenBootstrapped.git"
#   - DOXYGEN_BT_TAG : Git Tag of DoxygenBt. Default ! "v1.3.3".
#
#   - SHAREDCACHE_BUILD_TEST : Build SharedCache Test executable [ON OFF]. Default: OFF.
#   - GTEST_REPOSITORY : Repository of gtest, can be a local url [URL]. Default  https://github.com/google/googletest.git.
#   - GTEST_TAG : Version of gtest [master release-1.8.0]. Default : release-1.8.0.
#
## CMAKE OUTPUT
#
#   - SHAREDCACHE_TARGET : Output target to link to. Default: SharedCache
#

# ┌──────────────────────────────────────────────────────────────────┐
# │                       CMAKE PROPERTIES                           │
# └──────────────────────────────────────────────────────────────────┘

cmake_minimum_required(VERSION 3.11.0 FATAL_ERROR)

set(GTEST_REPOSITORY "https://github.com/google/googletest" CACHE STRING "Repository of googletest")
set(GTEST_TAG "master" CACHE STRING "Git tag/branch of googletest")

# ┌──────────────────────────────────────────────────────────────────┐
# │                       PROJECT SETTINGS                           │
# └──────────────────────────────────────────────────────────────────┘

# OPTIONS

set(SHAREDCACHE_VERSION_MAJOR 1)
set(SHAREDCACHE_VERSION_MINOR 0)
set(SHAREDCACHE_VERSION_PATCH 0)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    EXECUTE_PROCESS(
        COMMAND git describe --always
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE SHAREDCACHE_VERSION_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()
if(NOT SHAREDCACHE_VERSION_TAG)
    set(SHAREDCACHE_VERSION_TAG 00000000)
endif()
set(SHAREDCACHE_VERSION_TAG_HEX 0x${SHAREDCACHE_VERSION_TAG})
set(SHAREDCACHE_VERSION ${SHAREDCACHE_VERSION_MAJOR}.${SHAREDCACHE_VERSION_MINOR}.${SHAREDCACHE_VERSION_PATCH} CACHE STRING "" FORCE)

# General
set(SHAREDCACHE_PROJECT "SharedCache" CACHE STRING "Project Name")
set(SHAREDCACHE_TARGET ${SHAREDCACHE_PROJECT} CACHE STRING "SharedCache library name")
set(SHAREDCACHE_VERSION ${SHAREDCACHE_VERSION_MAJOR}.${SHAREDCACHE_VERSION_MINOR}.${SHAREDCACHE_VERSION_PATCH} CACHE STRING "SharedCache current version, this is only decorative and will not configure any files" FORCE)

set(SHAREDCACHE_FOLDER_PREFIX ${SHAREDCACHE_PROJECT} CACHE STRING "Prefix folder for all SharedCache generated targets in generated project (only decorative)")

set(SHAREDCACHE_USE_NAMESPACE ON CACHE BOOL "Should we use namespace")
set(SHAREDCACHE_NAMESPACE ${SHAREDCACHE_PROJECT} CACHE STRING "Namespace for the library")

# Tests
set(SHAREDCACHE_ENABLE_TESTS ON CACHE BOOL "Create or not a target for test (compatible with CTests)")
set(SHAREDCACHE_TESTS_PREFIX ${SHAREDCACHE_PROJECT} CACHE STRING "Prefix for all SharedCache tests")

# Install
set(SHAREDCACHE_ENABLE_INSTALL OFF CACHE BOOL "Create or not a target for install")
set(SHAREDCACHE_INSTALL_PREFIX ${SHAREDCACHE_PROJECT} CACHE STRING "Prefix for all SharedCache headers in the install folder")

# CREATE PROJECT

project(${SHAREDCACHE_PROJECT} VERSION ${SHAREDCACHE_VERSION} LANGUAGES C CXX)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 14)

# LOG OPTIONS

message(STATUS "---------------- SHAREDCACHE OPTIONS. ----------------")

#General
message(STATUS "SHAREDCACHE_PROJECT            : " ${SHAREDCACHE_PROJECT})
message(STATUS "SHAREDCACHE_TARGET             : " ${SHAREDCACHE_TARGET})
message(STATUS "SHAREDCACHE_VERSION            : " ${SHAREDCACHE_VERSION})
message(STATUS "SHAREDCACHE_VERSION_TAG        : " ${SHAREDCACHE_VERSION_TAG})
message(STATUS "SHAREDCACHE_FOLDER_PREFIX      : " ${SHAREDCACHE_FOLDER_PREFIX})

# Tests
message(STATUS "SHAREDCACHE_ENABLE_TESTS       : " ${SHAREDCACHE_ENABLE_TESTS})
if(SHAREDCACHE_ENABLE_TESTS)
message(STATUS "SHAREDCACHE_TESTS_PREFIX       : " ${SHAREDCACHE_TESTS_PREFIX})
endif(SHAREDCACHE_ENABLE_TESTS)

# Install
message(STATUS "SHAREDCACHE_ENABLE_INSTALL     : " ${SHAREDCACHE_ENABLE_INSTALL})
if(SHAREDCACHE_ENABLE_INSTALL)
message(STATUS "SHAREDCACHE_INSTALL_PREFIX     : " ${SHAREDCACHE_INSTALL_PREFIX})
endif(SHAREDCACHE_ENABLE_INSTALL)

message(STATUS "---------------- DONE WITH OPTIONS. -----------------")

# ┌──────────────────────────────────────────────────────────────────┐
# │                       Dependencies                               │
# └──────────────────────────────────────────────────────────────────┘

include(FetchContent)

# googletest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY ${GTEST_REPOSITORY}
    GIT_TAG        ${GTEST_TAG}
)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET ON)

# ┌──────────────────────────────────────────────────────────────────┐
# │                       SOURCES                                    │
# └──────────────────────────────────────────────────────────────────┘

if(SHAREDCACHE_ENABLE_QML)
    file(GLOB_RECURSE SHAREDCACHE_QML_SRCS qml/*.qml)

    foreach(SOURCE IN ITEMS ${SHAREDCACHE_QML_SRCS})
        get_filename_component(SOURCE_PATH "${SOURCE}" PATH)
        file(RELATIVE_PATH SOURCE_PATH_REL ${CMAKE_CURRENT_SOURCE_DIR}/qml "${SOURCE_PATH}")
        string(REPLACE "/" "\\" GROUP_PATH "${SOURCE_PATH_REL}")
        source_group("Qml\\${GROUP_PATH}" FILES "${SOURCE}")
    endforeach()

    # Add qml to the qtquick compiler
    qtquick_compiler_add_resources(SHAREDCACHE_QML_RES ${CMAKE_CURRENT_SOURCE_DIR}/qml/SharedCache.qrc)

    # Trick to rerun cmake each time SharedCache.qrc is updated
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/qml/SharedCache.qrc
      ${CMAKE_CURRENT_BINARY_DIR}/qml.junk
    )

    # Dont Run Moc on cpp file generated from qml
    foreach(SHAREDCACHE_CURRENT_QML_FILE ${SHAREDCACHE_QML_RES})
        set_property(SOURCE ${SHAREDCACHE_CURRENT_QML_FILE} PROPERTY SKIP_AUTOMOC ON)
        source_group("Qml\\QtQuickCompiler Files" FILES ${SHAREDCACHE_CURRENT_QML_FILE})
    endforeach()
endif() # SHAREDCACHE_ENABLE_QML

set(SHAREDCACHE_INCS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SHAREDCACHE_PRIV_INCS_DIR ${SHAREDCACHE_INCS_DIR}/SharedCache)

set(SHAREDCACHE_SRCS
    ${SHAREDCACHE_INCS_DIR}/SharedCache.hpp
    ${SHAREDCACHE_PRIV_INCS_DIR}/CircularCache.hpp
    ${SHAREDCACHE_PRIV_INCS_DIR}/ListCache.hpp
    ${SHAREDCACHE_PRIV_INCS_DIR}/private/BaseContainer.hpp
    ${SHAREDCACHE_PRIV_INCS_DIR}/private/Export.hpp
    )
source_group("SharedCache" FILES ${SHAREDCACHE_SRCS})

# ┌──────────────────────────────────────────────────────────────────┐
# │                       TARGET                                     │
# └──────────────────────────────────────────────────────────────────┘

# SHAREDCACHE TARGET
add_library(${SHAREDCACHE_TARGET} INTERFACE)
target_sources(${SHAREDCACHE_TARGET} INTERFACE "${SHAREDCACHE_SRCS}")

# Set include directory
target_include_directories(${SHAREDCACHE_TARGET} INTERFACE $<BUILD_INTERFACE:${SHAREDCACHE_INCS_DIR}> $<INSTALL_INTERFACE:${SHAREDCACHE_INSTALL_PREFIX}>)

# Version
target_compile_definitions(${SHAREDCACHE_TARGET} INTERFACE -DSHAREDCACHE_VERSION_MAJOR=${SHAREDCACHE_VERSION_MAJOR})
target_compile_definitions(${SHAREDCACHE_TARGET} INTERFACE -DSHAREDCACHE_VERSION_MINOR=${SHAREDCACHE_VERSION_MINOR})
target_compile_definitions(${SHAREDCACHE_TARGET} INTERFACE -DSHAREDCACHE_VERSION_PATCH=${SHAREDCACHE_VERSION_PATCH})
target_compile_definitions(${SHAREDCACHE_TARGET} INTERFACE -DSHAREDCACHE_VERSION_TAG=${SHAREDCACHE_VERSION_TAG})
target_compile_definitions(${SHAREDCACHE_TARGET} INTERFACE -DSHAREDCACHE_VERSION_TAG_HEX=${SHAREDCACHE_VERSION_TAG_HEX})

if(SHAREDCACHE_USE_NAMESPACE)
    target_compile_definitions(${SHAREDCACHE_TARGET} INTERFACE -DSHAREDCACHE_USE_NAMESPACE)
    target_compile_definitions(${SHAREDCACHE_TARGET} INTERFACE -DSHAREDCACHE_NAMESPACE=${SHAREDCACHE_NAMESPACE})
endif(SHAREDCACHE_USE_NAMESPACE)

# ┌──────────────────────────────────────────────────────────────────┐
# │                           TESTS                                  │
# └──────────────────────────────────────────────────────────────────┘

if(SHAREDCACHE_ENABLE_TESTS)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CTestLists.txt)
endif()

# ┌──────────────────────────────────────────────────────────────────┐
# │                         INSTALL                                  │
# └──────────────────────────────────────────────────────────────────┘

if(SHAREDCACHE_ENABLE_INSTALL)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CInstallLists.txt)
endif()