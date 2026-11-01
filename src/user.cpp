#include "user.h"

User::User(UserId id, const string& username, const string& password)
    : id(id), username(username), active(true),
      passwordhash(password_hasher::hashPassword(password)) {
}

UserId User::getId() const {
    return id;
}

const string& User::getusername() const {
    return username;
}

bool User::isActive() const {
    return active;
}

bool User::haspermission(Permission action) const {
    if (!isActive()) {
        return false;
    }

    switch (action) {
        case Permission::VIEW_COURSES:
        case Permission::VIEW_ASSIGNMENTS:
            return true;

        default:
            return false;
    }
}
bool User::verifyPassword(const string& password) const {
    return password_hasher::verifyPassword(password, passwordhash);
}