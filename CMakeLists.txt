
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

target_link_libraries(lms_test PRIVATE
    SQLite3::SQLite3
    "${ARGON2_LIBRARY}"
)

# GoogleTest
if(BUILD_TESTING)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    FetchContent_MakeAvailable(googletest)

    add_executable(lms_unit_tests
        tests/user_repository_tests.cpp
        tests/database_tests.cpp
        tests/user_integration_tests.cpp
        src/database.cpp
        src/sqlite_repository.cpp
        src/sqlite_user_repository.cpp
        src/password_hasher.cpp
    )

    target_include_directories(lms_unit_tests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${ARGON2_INCLUDE_DIR}"
    )

    target_link_libraries(lms_unit_tests PRIVATE
        GTest::gtest_main
        SQLite3::SQLite3
        "${ARGON2_LIBRARY}"
    )
target_compile_options(lms_unit_tests PRIVATE
    --coverage
    -O0
    -g
)

target_link_options(lms_unit_tests PRIVATE
    --coverage
)
    include(GoogleTest)
endif()
