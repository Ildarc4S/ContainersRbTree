cmake_minimum_required(VERSION 3.20)

file(GLOB TEST_SOURCES "*.cc")

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    
    target_include_directories(${TEST_NAME} PRIVATE 
        ${CMAKE_SOURCE_DIR}/src/include
    )
    
    target_link_libraries(${TEST_NAME} PRIVATE 
        GTest::GTest 
        GTest::Main 
        Threads::Threads
    )
    
    target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -Werror)
    
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    
endforeach()

