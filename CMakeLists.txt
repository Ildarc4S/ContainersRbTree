cmake_minimum_required(VERSION 3.20)
project(S21Containers LANGUAGES CXX)

option(COVERAGE "Enable code coverage reporting" OFF)

if(COVERAGE)
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory(src/tests)

if(COVERAGE)
    find_program(LCOV lcov)
    find_program(GENHTML genhtml)
    
    if(LCOV AND GENHTML)
        add_custom_target(coverage
            COMMAND ${LCOV} 
              --capture --directory ${CMAKE_BINARY_DIR} 
              --output-file coverage.info 
              --ignore-errors inconsistent --rc geninfo_unexecuted_blocks=1
            COMMAND ${LCOV} 
              --remove coverage.info '/usr/*' '${CMAKE_BINARY_DIR}/src/tests/*' 
              --output-file coverage.filtered.info 
              --ignore-errors unused
            COMMAND ${GENHTML} 
              coverage.filtered.info 
              --output-directory coverage
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report"
        )
    endif()
endif()

