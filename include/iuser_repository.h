#ifndef IUSER_REPOSITORY_H
#define IUSER_REPOSITORY_H

#include "user.h"
#include <optional>
#include <functional>

class IUserRepository {
public:
    virtual ~IUserRepository() = default;

    virtual std::optional<std::reference_wrapper<User>>
    findByUsername(const string& username) = 0;
};

#endif
