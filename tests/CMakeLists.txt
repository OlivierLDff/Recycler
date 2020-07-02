#
#   Recycler CTest script
#
#   Copyright Olivier Le Doeuff 2019
#

set(RECYCLER_TESTS ${RECYCLER_TARGET}_Tests)
set(RECYCLER_BENCHMARK ${RECYCLER_TARGET}_CircularBenchmark)

add_executable(${RECYCLER_TESTS} Main.cpp
  CircularTests.cpp
  BufferTests.cpp
)
add_executable(${RECYCLER_BENCHMARK} CircularBenchmark.cpp)

target_link_libraries(${RECYCLER_TESTS}          ${RECYCLER_TARGET} gtest)
target_link_libraries(${RECYCLER_BENCHMARK}      ${RECYCLER_TARGET})

target_include_directories(${RECYCLER_TESTS}     PRIVATE include)
target_include_directories(${RECYCLER_BENCHMARK} PRIVATE include)

if(RECYCLER_FOLDER_PREFIX)
  set_target_properties(${RECYCLER_TESTS}        PROPERTIES FOLDER ${RECYCLER_FOLDER_PREFIX}/Tests)
  set_target_properties(${RECYCLER_BENCHMARK}    PROPERTIES FOLDER ${RECYCLER_FOLDER_PREFIX}/Tests)
endif()

message(STATUS "Add Test: ${RECYCLER_TESTS}")
add_test(NAME ${RECYCLER_TESTS} COMMAND ${RECYCLER_TESTS})
add_test(NAME ${RECYCLER_BENCHMARK} COMMAND ${RECYCLER_BENCHMARK})