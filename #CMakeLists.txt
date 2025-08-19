cmake_minimum_required(VERSION 3.20)
project(S21Containers LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(COVERAGE "Enable coverage reporting" OFF)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# Список тестируемых контейнеров
set(CONTAINERS rb_tree)

# Источники и заголовки тестов
file(GLOB TEST_SOURCE_FILES
    "${CMAKE_SOURCE_DIR}/src/tests/*.cc"
    "${CMAKE_SOURCE_DIR}/src/tests/*.h"
)

# Папки с заголовками библиотеки (header-only)
set(LIB_HEADER_DIRS
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/include"
)

enable_testing()

# Добавляем тестовые исполняемые файлы
foreach(CONTAINER ${CONTAINERS})
    set(TEST_NAME "s21_test_${CONTAINER}")
    
    add_executable(${TEST_NAME} src/tests/${TEST_NAME}.cc)
    
    # Добавляем include для заголовков библиотеки
    target_include_directories(${TEST_NAME} PRIVATE ${LIB_HEADER_DIRS})

    # Линкуем библиотеки GoogleTest, Threads
    target_link_libraries(${TEST_NAME} PRIVATE GTest::GTest GTest::Main Threads::Threads)
 
    # Добавляем тест в CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    # Цель для запуска теста вручную
    add_custom_target(run_${TEST_NAME}
        COMMAND ./${TEST_NAME}
        DEPENDS ${TEST_NAME}
        COMMENT "Running ${TEST_NAME} tests"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -Werror)

    # Добавляем coverage флаги, если покрытие включено
    if(COVERAGE)
        target_compile_options(${TEST_NAME} PRIVATE --coverage -O0 -g)
        target_link_options(${TEST_NAME} PRIVATE --coverage)
    endif()
endforeach()

# --- clang-tidy ---
find_program(CLANG_TIDY "clang-tidy-18")
if(CLANG_TIDY)
    file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy
        "---
Checks: >
    -*,
    clang-analyzer-*,
    google-*,
    modernize-*,
    performance-*,
    readability-*,
    bugprone-*
    -modernize-use-auto,
    -modernize-use-trailing-return-type
WarningsAsErrors: '*'
HeaderFilterRegex: '.*'
...
")
    add_custom_target(clang-tidy-test
        COMMAND ${CLANG_TIDY}
        ${TEST_SOURCE_FILES}
        --config-file=${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy
        -p=${CMAKE_BINARY_DIR}
        --quiet
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running clang-tidy static analysis"
        VERBATIM
    )
else()
    message(WARNING "clang-tidy not found! Static analysis target 'clang-tidy' will not be available")
endif()

# --- cppcheck ---
find_program(CPPCHECK "cppcheck")
if(CPPCHECK)
    add_custom_target(cppcheck-test
        COMMAND ${CPPCHECK}
            --enable=warning,performance,portability,style
            --suppress=missingIncludeSystem
            --std=c++20
            --template="[{severity}][{id}] {message} {file}:{line}"
            --error-exitcode=1
            --inline-suppr
            --project=${CMAKE_BINARY_DIR}/compile_commands.json
            -i ${CMAKE_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running cppcheck static analysis"
        VERBATIM
    )
    add_custom_target(cppcheck-test-sources
        COMMAND ${CPPCHECK}
            --enable=all
            --suppress=missingIncludeSystem
            --std=c++20
            --template="[{severity}][{id}] {message} {file}:{line}"
            --error-exitcode=1
            --inline-suppr
            ${TEST_SOURCE_FILES}
            -i ${CMAKE_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running cppcheck on source files directly"
        VERBATIM
    )
else()
    message(WARNING "cppcheck not found! Static analysis targets 'cppcheck' and 'cppcheck-sources' will not be available")
endif()

# --- Valgrind ---
find_program(VALGRIND "valgrind")
if(VALGRIND)
    set(TEST_EXECUTABLES)
    foreach(CONTAINER ${CONTAINERS})
        list(APPEND TEST_EXECUTABLES "s21_test_${CONTAINER}")
    endforeach()

    add_custom_target(valgrind-test
        COMMAND ${CMAKE_COMMAND} -E echo "=== Running all tests with Valgrind ==="
        COMMAND ${VALGRIND}
            --leak-check=full --show-leak-kinds=all --track-origins=yes
            --error-exitcode=1
            --log-file=${CMAKE_BINARY_DIR}/valgrind.log
            ${CMAKE_CTEST_COMMAND} --output-on-failure
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running all tests with Valgrind memory checker"
        DEPENDS ${TEST_EXECUTABLES}
    )
    foreach(CONTAINER ${CONTAINERS})
        set(TEST_NAME "s21_test_${CONTAINER}")
        add_custom_target(valgrind_${TEST_NAME}
            COMMAND ${VALGRIND}
                --leak-check=full --show-leak-kinds=all --track-origins=yes
                --error-exitcode=1
                --log-file=${CMAKE_BINARY_DIR}/valgrind_${TEST_NAME}.log
                ./${TEST_NAME}
            DEPENDS ${TEST_NAME}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running ${TEST_NAME} with Valgrind"
        )
    endforeach()
else()
    message(WARNING "Valgrind not found! Memory checking targets will not be available")
endif()

# --- clang-format ---
find_program(CLANG_FORMAT "clang-format-18")
if(CLANG_FORMAT)
    add_custom_target(clang-format-test
        COMMAND ${CLANG_FORMAT} --style=Google -n --verbose ${TEST_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Checking code formatting with clang-format (Google Style)"
    )
    add_custom_target(clang-format-fix
        COMMAND ${CLANG_FORMAT} --style=Google -i ${TEST_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Formatting code with clang-format (Google Style)"
    )
else()
    message(WARNING "clang-format not found. clang-format-check and clang-format targets will not be available.")
endif()

# --- Общая цель покрытия coverage_all ---
find_program(LCOV lcov)
find_program(GENHTML genhtml)

set(COVERAGE_DIR "${CMAKE_BINARY_DIR}/coverage")

if(COVERAGE AND LCOV AND GENHTML)
    set(COVERAGE_TARGETS)
    foreach(CONTAINER ${CONTAINERS})
        list(APPEND COVERAGE_TARGETS run_s21_test_${CONTAINER})
    endforeach()

    add_custom_target(coverage_all
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${COVERAGE_DIR}
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
        COMMAND ctest --output-on-failure
        COMMAND ${LCOV} --capture --directory ${CMAKE_BINARY_DIR} --output-file coverage.info --ignore-errors inconsistent --rc geninfo_unexecuted_blocks=1
        COMMAND ${LCOV} --remove coverage.info '/usr/*' --output-file coverage_filtered.info
        COMMAND ${GENHTML} --output-directory ${COVERAGE_DIR} coverage_filtered.info
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating coverage report"
        DEPENDS ${COVERAGE_TARGETS}
    )
else()
    message(WARNING "lcov or genhtml not found - coverage targets disabled")
endif()

