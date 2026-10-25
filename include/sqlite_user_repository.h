#ifndef SQLITE_USER_REPOSITORY_H
#define SQLITE_USER_REPOSITORY_H

#include "sqlite_repository.h"
#include "user.h"
#include <optional>
#include <string>

class SQLiteUserRepository : public SQLiteRepository {
public:
    explicit SQLiteUserRepository(SQLiteConnection& connection);

    void createTable();

    bool saveUser(
        UserId id,
        const std::string& username,
        const std::string& password,
        const std::string& role
    );

    bool usernameExists(const std::string& username);
    bool userIdExists(UserId id);
    bool administratorExists();
    std::optional<std::string> authenticateRole(const std::string& username,const std::string& password);
};

#endif
