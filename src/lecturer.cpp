#include "lecturer.h"

Lecturer::Lecturer(UserId id, const string& username,const string& passwordhash): User(id, username, passwordhash) {
}

bool Lecturer::haspermission(Permission action) const {
    if (!isActive()) {
    return false;
    }
    switch (action) {
        case Permission::VIEW_COURSES:
        case Permission::VIEW_ASSIGNMENTS:
        case Permission::CREATE_ASSIGNMENT:
        case Permission::MARK_SUBMISSION:
        case Permission::GRANT_EXTENSIONS:
            return true;

        default:
            return false;
    }
}