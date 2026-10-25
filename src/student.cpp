#include "student.h"

Student::Student(UserId id, const string& username, const string& passwordhash)
    : User(id, username, passwordhash) {
}

bool Student::haspermission(Permission action) const {
    if (!isActive()) {
    return false;
    }
    switch (action) {
        case Permission::VIEW_COURSES:
        case Permission::VIEW_ASSIGNMENTS:
        case Permission::SUBMIT_WORK:

            return true;

        default:
            return false;
    }
}

void Student::addEnrollment(const Enrollment& enrollment) {
    enrollments.push_back(enrollment);
}