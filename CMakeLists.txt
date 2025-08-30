cmake_minimum_required(VERSION 3.30)
project(S21Containers LANGUAGES CXX)

option(COVERAGE "Enable code coverage reporting" OFF)

if(COVERAGE)
  add_compile_options(--coverage -O0 -g)
  add_link_options(--coverage)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FORMAT_SOURCES
  ${CMAKE_SOURCE_DIR}/src/include/*.h
  ${CMAKE_SOURCE_DIR}/src/include/rb_tree/*.h
  ${CMAKE_SOURCE_DIR}/src/tests/containers/*.cc
  ${CMAKE_SOURCE_DIR}/src/tests/rb_tree/*.cc
)

include(CTest)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

find_program(CLANG_FORMAT clang-format-18)
find_program(LCOV lcov)
find_program(GENHTML genhtml)
find_program(VALGRIND valgrind)

add_library(s21_containers INTERFACE)
target_include_directories(s21_containers INTERFACE
  ${CMAKE_SOURCE_DIR}/src/include
  ${CMAKE_SOURCE_DIR}/src/include/rb_tree
)

add_subdirectory(src/tests)

add_custom_target(coverage
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
  COMMAND ${LCOV} --capture --directory ${CMAKE_BINARY_DIR} --output-file coverage.info --ignore-errors inconsistent --rc geninfo_unexecuted_blocks=1 --quiet 2>/dev/null
  COMMAND ${LCOV} --remove coverage.info '/usr/*' '${CMAKE_SOURCE_DIR}/src/tests/*' --output-file coverage.filtered.info --ignore-errors inconsistent --quiet 2>/dev/null
  COMMAND ${GENHTML}
    coverage.filtered.info
    --output-directory coverage
    -c ${CMAKE_SOURCE_DIR}/gcov/gcov.css
    --quiet
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Generating code coverage report"
)

add_custom_target(clang-format-test
  COMMAND ${CLANG_FORMAT} --style=Google --dry-run --Werror ${FORMAT_SOURCES}
  COMMENT "Checking code formatting with clang-format (Google style)"
)

add_custom_target(clang-format-fix
  COMMAND ${CLANG_FORMAT} --style=Google -i ${FORMAT_SOURCES}
  COMMENT "Fixing code formatting with clang-format (Google style)"
)

add_custom_target(valgrind-test
  COMMAND ${VALGRIND} --leak-check=full --error-exitcode=1 --track-origins=yes ${CMAKE_CTEST_COMMAND} --output-on-failure
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Running tests with Valgrind memory checker"
)