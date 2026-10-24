cmake_minimum_required(VERSION 3.20)
project(LearningManagementSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SQLite3 REQUIRED)
find_path(ARGON2_INCLUDE_DIR argon2.h REQUIRED)
find_library(ARGON2_LIBRARY NAMES argon2 REQUIRED)

file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(lms_test ${SOURCES})

target_include_directories(lms_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${ARGON2_INCLUDE_DIR}"
)

target_link_libraries(lms_test PRIVATE SQLite3::SQLite3 "${ARGON2_LIBRARY}")
