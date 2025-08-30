set(CONTAINER_TESTS
  s21_test_set.cc
  s21_test_map.cc
  s21_test_multiset.cc
)

foreach(TEST_SOURCE ${CONTAINER_TESTS})
  get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

  add_executable(${TEST_NAME} ${TEST_SOURCE})

  target_include_directories(${TEST_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/rb_tree
  )

  target_link_libraries(${TEST_NAME} PRIVATE
    GTest::gtest_main
    Threads::Threads
  )

  target_compile_options(${TEST_NAME} PRIVATE
    -Wall -Wextra -Werror
  )

  add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()