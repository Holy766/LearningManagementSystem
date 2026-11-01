
#include "user_repository.h"
#include "password_hasher.h"
#include <algorithm>

#include <stdexcept>
#include <utility>
#include <cctype>

void UserRepository::save(std::unique_ptr<User> user) {
    if (!user) {
        throw std::invalid_argument("Cannot save a null user.");
    }
    string username = user->getusername();

bool hasNonSpace = std::any_of(
    username.begin(),
    username.end(),
    [](char ch) {
        return !std::isspace(static_cast<unsigned char>(ch));
    }
);
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
    auto it = std::find_if(
        users.begin(),
        users.end(),
        [&username](const auto& user) {
            return user->getusername() == username;
        }
    );

    if (it != users.end()) {
        return std::ref(**it);
    }

    return std::nullopt;
}

User* UserRepository::findById(UserId id) const {
    auto it = std::find_if(
        users.begin(),
        users.end(),
        [id](const auto& user) {
            return user->getId() == id;
        }
    );

    if (it != users.end()) {
        return it->get();
    }

    return nullptr;
}
bool UserRepository::remove(UserId id) {
    auto it = std::find_if(
        users.begin(),
        users.end(),
        [id](const auto& user) {
            return user->getId() == id;
        }
    );

    if (it != users.end()) {
        users.erase(it);
        return true;
    }

    return false;
}
bool UserRepository::usernameExists(const string& username) const {
    return std::any_of(
        users.begin(),
        users.end(),
        [&username](const auto& user) {
            return user->getusername() == username;
        }
    );
}
bool UserRepository::idExists(UserId id) const {
    return findById(id) != nullptr;
}

User* UserRepository::authenticate(
    const string& username,
    const string& password
) const {
    auto it = std::find_if(
        users.begin(),
        users.end(),
        [&username](const auto& user) {
            return user->getusername() == username;
        }
    );

    if (it == users.end()) {
        return nullptr;
    }

    if (!(*it)->isActive()) {
        return nullptr;
    }

    if ((*it)->verifyPassword(password)) {
        return it->get();
    }

    return nullptr;
}