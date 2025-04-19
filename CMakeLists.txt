cmake_minimum_required(VERSION 3.28)
project(S21Containers LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# Список тестируемых контейнеров
set(CONTAINERS 
    array
    vector
    list
    map
    set
    stack
    queue
    multiset
)

# Список тестовых файлов
file(GLOB TEST_SOURCE_FILES
    "${CMAKE_SOURCE_DIR}/tests/*.cc"
    "${CMAKE_SOURCE_DIR}/tests/*.h"
)

# Список файлов библиотеки
file(GLOB LIB_SOURCE_FILES
    "${CMAKE_SOURCE_DIR}/*.cc"
    "${CMAKE_SOURCE_DIR}/*.h"
)

enable_testing()

foreach(CONTAINER ${CONTAINERS})
    # Имя тестового исполняемого файла
    set(TEST_NAME "s21_test_${CONTAINER}")
    
    # Создание исполняемого файла
    add_executable(${TEST_NAME} tests/${TEST_NAME}.cc)
    
    # Подключение зависимостей
    target_link_libraries(${TEST_NAME}
        PRIVATE 
        GTest::GTest
        GTest::Main
        Threads::Threads
    )
    
    # Добавление в CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    
    # Создание цели для запуска
    add_custom_target(run_${TEST_NAME}
        COMMAND ./${TEST_NAME}
        DEPENDS ${TEST_NAME}
        COMMENT "Running ${TEST_NAME} tests"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    
    target_compile_options(${TEST_NAME} PRIVATE
        -Wall
        -Wextra
        -Werror
    )
endforeach()

# Поиск clang-tidy и настройка анализа
find_program(CLANG_TIDY "clang-tidy")
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
        ${LIB_SOURCE_FILES} ${TEST_SOURCE_FILES}
        --config-file=${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy
        -p=${CMAKE_BINARY_DIR}  # Указываем путь к compile_commands.json
        --quiet
        COMMENT "Running clang-tidy static analysis"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        VERBATIM
    )
else()
    message(WARNING "clang-tidy not found! Static analysis target 'clang-tidy' will not be available")
endif()


find_program(CPPCHECK "cppcheck")
if(CPPCHECK)
    # Цель для запуска cppcheck с базовыми настройками
    add_custom_target(cppcheck-test
        COMMAND ${CPPCHECK}
            --enable=warning,performance,portability,style
            --suppress=missingIncludeSystem
            --std=c++20
            --template="[{severity}][{id}] {message} {file}:{line}"
            --error-exitcode=1
            --inline-suppr
            --project=${CMAKE_BINARY_DIR}/compile_commands.json
            -i ${CMAKE_BINARY_DIR}  # Игнорируем build-директорию
        COMMENT "Running cppcheck static analysis"
        VERBATIM
    )

    # Альтернативная цель для проверки только исходников (без compile_commands.json)
    add_custom_target(cppcheck-test-sources
        COMMAND ${CPPCHECK}
            --enable=all
            --suppress=missingIncludeSystem
            --std=c++20
            --template="[{severity}][{id}] {message} {file}:{line}"
            --error-exitcode=1
            --inline-suppr
            ${LIB_SOURCE_FILES} ${TEST_SOURCE_FILES}
            -i ${CMAKE_BINARY_DIR}
        COMMENT "Running cppcheck on source files directly"
        VERBATIM
    )
else()
    message(WARNING "cppcheck not found! Static analysis targets 'cppcheck' and 'cppcheck-sources' will not be available")
endif()


find_program(VALGRIND "valgrind")
if(VALGRIND)
    # Создаем список всех тестовых исполняемых файлов
    set(TEST_EXECUTABLES)
    foreach(CONTAINER ${CONTAINERS})
        list(APPEND TEST_EXECUTABLES "s21_test_${CONTAINER}")
    endforeach()

    # Основная цель для запуска всех тестов через Valgrind
    add_custom_target(valgrind-test
        COMMAND ${CMAKE_COMMAND} -E echo "=== Running all tests with Valgrind ==="
        COMMAND ${VALGRIND}
            --leak-check=full
            --show-leak-kinds=all
            --track-origins=yes
            --error-exitcode=1
            --log-file=${CMAKE_BINARY_DIR}/valgrind.log
            ${CMAKE_CTEST_COMMAND} --output-on-failure
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running all tests with Valgrind memory checker"
        DEPENDS ${TEST_EXECUTABLES}  # Зависит от скомпилированных тестов
    )

    # Альтернативная цель для запуска отдельных тестов
    foreach(CONTAINER ${CONTAINERS})
        set(TEST_NAME "s21_test_${CONTAINER}")
        add_custom_target(valgrind_${TEST_NAME}
            COMMAND ${VALGRIND}
                --leak-check=full
                --show-leak-kinds=all
                --track-origins=yes
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


# Поиск clang-format
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    # Цель для проверки форматирования (clang-format-check)
    add_custom_target(clang-format-test
        COMMAND ${CLANG_FORMAT} --style=Google -n --verbose ${LIB_SOURCE_FILES} ${TEST_SOURCE_FILES}
        COMMENT "Checking code formatting with clang-format (Google Style)"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    # Цель для форматирования кода (clang-format)
    add_custom_target(clang-format-fix
        COMMAND ${CLANG_FORMAT} --style=Google -i ${LIB_SOURCE_FILES} ${TEST_SOURCE_FILES}
        COMMENT "Formatting code with clang-format (Google Style)"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
else()
    message(WARNING "clang-format not found. clang-format-check and clang-format targets will not be available.")
endif()


foreach(CONTAINER ${CONTAINERS})
    # Coverage-версия теста
    set(COVERAGE_TEST_NAME "s21_coverage_${CONTAINER}")
    
    add_executable(${COVERAGE_TEST_NAME} tests/s21_test_${CONTAINER}.cc)
    
    target_link_libraries(${COVERAGE_TEST_NAME}
        PRIVATE 
        GTest::GTest
        GTest::Main
        Threads::Threads
    )
    
    target_compile_options(${COVERAGE_TEST_NAME} PRIVATE
        -Wall
        -Wextra
        -Werror
        --coverage
    )
    
    target_link_options(${COVERAGE_TEST_NAME} PRIVATE
        --coverage
    )

    target_include_directories(${COVERAGE_TEST_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}
    )
    
    add_custom_target(run_coverage_${CONTAINER}
        COMMAND ./${COVERAGE_TEST_NAME}
        DEPENDS ${COVERAGE_TEST_NAME}
        COMMENT "Running coverage tests for ${CONTAINER}"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endforeach()

# Глобальная цель coverage
find_program(LCOV_PATH lcov)
find_program(GENHTML_PATH genhtml)

set(COVERAGE_DIR "${CMAKE_SOURCE_DIR}/coverage")

if(LCOV_PATH AND GENHTML_PATH)
    set(COVERAGE_TARGETS)
    foreach(CONTAINER ${CONTAINERS})
        list(APPEND COVERAGE_TARGETS run_coverage_${CONTAINER})
    endforeach()

    add_custom_target(coverage_all
        COMMAND find . -name "*.gcda" -delete
        
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${COVERAGE_TARGETS}
    
        COMMAND lcov --capture --directory . --output-file coverage.info -ignore-errors mismatch 

        COMMAND lcov --remove coverage.info '/usr/include/*' --output-file coverage_filtered.info

        COMMAND genhtml --output-directory ${COVERAGE_DIR} coverage_filtered.info
        
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating coverage report"
        DEPENDS ${COVERAGE_TARGETS}
    )
else()
    message(WARNING "lcov or genhtml not found - coverage targets disabled")
endif()
