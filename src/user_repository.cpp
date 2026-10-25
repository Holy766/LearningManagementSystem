
#include "user_repository.h"
#include "password_hasher.h"

#include <stdexcept>
#include <utility>
#include <cctype>

void UserRepository::save(std::unique_ptr<User> user) {
    if (!user) {
        throw std::invalid_argument("Cannot save a null user.");
    }
    bool hasNonSpace = false;

    for (char ch : user->getusername()) {
    if (!std::isspace(static_cast<unsigned char>(ch))) {
        hasNonSpace = true;
        break;
     }
    }

    if (!hasNonSpace) {
    throw std::invalid_argument("Username cannot be empty.");
    }
    if (idExists(user->getId())) {
        throw std::invalid_argument("A user with this ID already exists.");
    }

    if (usernameExists(user->getusername())) {
        throw std::invalid_argument("A user with this username already exists.");
    }

    users.push_back(std::move(user));
}

std::optional<std::reference_wrapper<User>>
UserRepository::findByUsername(const string& username) {
    for (const auto& user : users) {
        if (user->getusername() == username) {
            return std::ref(*user);
        }
    }

    return std::nullopt;
}

User* UserRepository::findById(UserId id) const {
    for (const auto& user : users) {
        if (user->getId() == id) {
            return user.get();
        }
    }

    return nullptr;
}

bool UserRepository::remove(UserId id) {
    for (auto it = users.begin(); it != users.end(); ++it) {
        if ((*it)->getId() == id) {
            users.erase(it);
            return true;
        }
    }

    return false;
}

bool UserRepository::usernameExists(const string& username) const {
    for (const auto& user : users) {
        if (user->getusername() == username) {
            return true;
        }
    }

    return false;
}

bool UserRepository::idExists(UserId id) const {
    return findById(id) != nullptr;
}

User* UserRepository::authenticate(
    const string& username,
    const string& password
) const {
    for (const auto& user : users) {
        if (user->getusername() == username) {
            if (!user->isActive()) {
                return nullptr;
            }

            if (user->verifyPassword(password)) {
                return user.get();
            }

            return nullptr;
        }
    }

    return nullptr;
}

std::size_t UserRepository::getSize() const {
    return users.size();
}
