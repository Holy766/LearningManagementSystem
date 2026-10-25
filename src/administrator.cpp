
#include "administrator.h"

Administrator::Administrator(UserId id, const string& username, const string& passwordhash): User(id, username, passwordhash) {
}

bool Administrator::haspermission(Permission action) const {
    if (!isActive()) {
    return false;
    }
    switch (action) {
        case Permission::VIEW_COURSES:
        case Permission::VIEW_ASSIGNMENTS:
        case Permission::MANAGE_USERS:
        case Permission::MANAGE_DEPARTMENTS:
        case Permission::MANAGE_COURSES:
        case Permission::ASSIGN_LECTURERS:
        case Permission::ENROLL_STUDENTS:
            return true;

        default:
            return false;
    }
}
