# LearningManagementSystem
offline Learning management system
# How to compile
Make sure CMake, a C++ compiler, SQLite3, and the Argon2 library are installed and configured.

cmake -s . -B build
cmake --build build

# How to run
.\build\lms_test.exe

# Initial administrator  setup
username:  Mokhaka
password:  phalali09




## Version 1.1 – Class-wide deadline extensions

Version 1.1 adds a lecturer option to extend an assignment deadline for one student or for all students enrolled in the assignment's course. The revised deadline is displayed on each affected student's assignment list.

The Phase 8 regression test can be built and run with:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```
