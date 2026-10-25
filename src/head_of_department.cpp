
#include "head_of_department.h"

HoD::HoD(UserId id, const string& username,const string& passwordhash): User(id, username, passwordhash) {
}

bool HoD::haspermission(Permission action) const {
    if (!isActive()) {
    return false;
    }
    switch (action) {
        case Permission::VIEW_COURSES:
        case Permission::VIEW_ASSIGNMENTS:
        case Permission::MODERATE_MARKS:
        case Permission::RELEASE_MARKS:
            return true;

        default:
            return false;
    }
}
