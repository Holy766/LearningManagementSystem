
#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include "iuser_repository.h"
#include <memory>
#include <vector>

class UserRepository : public IUserRepository {
private:
    std::vector<std::unique_ptr<User>> users;

public:
    UserRepository() = default;

    UserRepository(const UserRepository&) = delete;
    UserRepository& operator=(const UserRepository&) = delete;

    void save(std::unique_ptr<User> user);

    std::optional<std::reference_wrapper<User>>
    findByUsername(const string& username) override;

    User* findById(UserId id) const;

    bool remove(UserId id);

    bool usernameExists(const string& username) const;

    bool idExists(UserId id) const;

    User* authenticate(
        const string& username,
        const string& password
    ) const;

    std::size_t getSize() const;
};

#endif
