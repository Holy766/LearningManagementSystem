
cmake_minimum_required(VERSION 3.20)
project(LearningManagementSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
include(FetchContent)

find_package(SQLite3 REQUIRED)
find_path(ARGON2_INCLUDE_DIR argon2.h REQUIRED)
find_library(ARGON2_LIBRARY NAMES argon2 REQUIRED)

file(GLOB SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Main application
add_executable(lms_test ${SOURCES})

target_include_directories(lms_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${ARGON2_INCLUDE_DIR}"
)

target_link_libraries(lms_test PRIVATE SQLite::SQLite3 "${ARGON2_LIBRARY}")

# Phase 8 regression tests for individual and class-wide extensions.
enable_testing()
add_executable(class_extension_tests
    tests/class_extension_tests.cpp
    src/database.cpp
    src/sqlite_repository.cpp
    src/sqlite_extension_repository.cpp
    src/extension.cpp
    src/datetime.cpp
)
target_include_directories(class_extension_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(class_extension_tests PRIVATE SQLite::SQLite3)
add_test(NAME ClassExtensionRegressionTests COMMAND class_extension_tests)

# Run the existing GoogleTest suite when GoogleTest is available (as in CI).
find_package(GTest QUIET)
if(GTest_FOUND)
    file(GLOB_RECURSE EXISTING_TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )
    list(REMOVE_ITEM EXISTING_TEST_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/class_extension_tests.cpp"
    )

    set(TEST_LIBRARY_SOURCES ${SOURCES})
    list(FILTER TEST_LIBRARY_SOURCES EXCLUDE REGEX "/main\\.cpp$")

    add_executable(lms_existing_tests
        ${EXISTING_TEST_SOURCES}
        ${TEST_LIBRARY_SOURCES}
    )
    target_include_directories(lms_existing_tests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${ARGON2_INCLUDE_DIR}"
    )
    target_link_libraries(lms_existing_tests PRIVATE
        SQLite::SQLite3 "${ARGON2_LIBRARY}" GTest::gtest_main
    )
    add_test(NAME ExistingRegressionTests COMMAND lms_existing_tests)
endif()
