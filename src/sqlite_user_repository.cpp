#include "sqlite_user_repository.h"
#include "password_hasher.h"
#include <stdexcept>

#include <sqlite3.h>
#include <string>

SQLiteUserRepository::SQLiteUserRepository(SQLiteConnection& connection)
    : SQLiteRepository(connection) {
}

void SQLiteUserRepository::createTable() {
    const std::string sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY, "
        "username TEXT NOT NULL UNIQUE, "
        "password_hash TEXT NOT NULL, "
        "role TEXT NOT NULL CHECK(role IN "
        "('Student', 'Lecturer', 'HoD', 'Administrator')), "
        "active INTEGER NOT NULL DEFAULT 1"
        ");";

    connection.execute(sql);
}


bool SQLiteUserRepository::saveUser(
    UserId id,
    const std::string& username,
    const std::string& password,
    const std::string& role
) {
    if (id <= 0 || username.empty() || password.empty()) {
        return false;
    }

    if (role != "Student" &&
        role != "Lecturer" &&
        role != "HoD" &&
        role != "Administrator") {
        return false;
    }

    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "INSERT INTO users (id, username, password_hash, role) "
        "VALUES (?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK) {
        throw std::runtime_error(
            "Failed to prepare user registration: " +
            std::string(sqlite3_errmsg(db))
        );
    }

    const std::string passwordHash =password_hasher::hashPassword(password);

    sqlite3_bind_int(statement, 1, id);
    sqlite3_bind_text(statement, 2, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(
        statement, 3, passwordHash.c_str(), -1, SQLITE_TRANSIENT
    );
    sqlite3_bind_text(statement, 4, role.c_str(), -1, SQLITE_TRANSIENT);

    int result = sqlite3_step(statement);

    if (result != SQLITE_DONE) {
        std::string error = sqlite3_errmsg(db);
        sqlite3_finalize(statement);

        if (result == SQLITE_CONSTRAINT) {
            return false;
        }

        throw std::runtime_error(
            "Failed to register user: " + error
        );
    }

    sqlite3_finalize(statement);
    return true;
}


bool SQLiteUserRepository::usernameExists(const std::string& username) {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "SELECT 1 FROM users WHERE username = ? LIMIT 1;";

    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK) {
        throw std::runtime_error(
            "Failed to check username: " + std::string(sqlite3_errmsg(db))
        );
    }

    sqlite3_bind_text(
        statement, 1, username.c_str(), -1, SQLITE_TRANSIENT
    );

    int result = sqlite3_step(statement);

    if (result != SQLITE_ROW && result != SQLITE_DONE) {
        std::string error = sqlite3_errmsg(db);
        sqlite3_finalize(statement);
        throw std::runtime_error("Failed to check username: " + error);
    }

    bool exists = (result == SQLITE_ROW);

    sqlite3_finalize(statement);
    return exists;
}


bool SQLiteUserRepository::userIdExists(UserId id) {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "SELECT 1 FROM users WHERE id = ? LIMIT 1;";

    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK) {
        throw std::runtime_error(
            "Failed to check user ID: " + std::string(sqlite3_errmsg(db))
        );
    }

    sqlite3_bind_int(statement, 1, id);

    int result = sqlite3_step(statement);

    if (result != SQLITE_ROW && result != SQLITE_DONE) {
        std::string error = sqlite3_errmsg(db);
        sqlite3_finalize(statement);
        throw std::runtime_error("Failed to check user ID: " + error);
    }

    bool exists = (result == SQLITE_ROW);

    sqlite3_finalize(statement);
    return exists;
}


bool SQLiteUserRepository::administratorExists() {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "SELECT 1 FROM users "
        "WHERE role = 'Administrator' LIMIT 1;";

    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK) {
        throw std::runtime_error(
            "Failed to check administrator: " +
            std::string(sqlite3_errmsg(db))
        );
    }

    int result = sqlite3_step(statement);

    if (result != SQLITE_ROW && result != SQLITE_DONE) {
        std::string error = sqlite3_errmsg(db);
        sqlite3_finalize(statement);

        throw std::runtime_error(
            "Failed to check administrator: " + error
        );
    }

    bool exists = (result == SQLITE_ROW);

    sqlite3_finalize(statement);
    return exists;
}


std::optional<std::string> SQLiteUserRepository::authenticateRole(
    const std::string& username,
    const std::string& password
) {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "SELECT password_hash, role FROM users "
        "WHERE username = ? AND active = 1 LIMIT 1;";

    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK) {
        throw std::runtime_error(
            "Failed to prepare login query: " +
            std::string(sqlite3_errmsg(db))
        );
    }

    sqlite3_bind_text(
        statement, 1, username.c_str(), -1, SQLITE_TRANSIENT
    );

    int result = sqlite3_step(statement);
    std::optional<std::string> role;

    if (result == SQLITE_ROW) {
        const auto* storedHash =
            reinterpret_cast<const char*>(
                sqlite3_column_text(statement, 0)
            );

        const auto* storedRole =
            reinterpret_cast<const char*>(
                sqlite3_column_text(statement, 1)
            );

        if (storedHash != nullptr &&
            storedRole != nullptr &&
            password_hasher::verifyPassword(password, storedHash)) {
            role = std::string(storedRole);
        }
    } else if (result != SQLITE_DONE) {
        std::string error = sqlite3_errmsg(db);
        sqlite3_finalize(statement);

        throw std::runtime_error(
            "Failed to execute login query: " + error
        );
    }

    sqlite3_finalize(statement);
    return role;
}

std::optional<UserId> SQLiteUserRepository::getUserIdByUsername(
    const std::string& username
) {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "SELECT id FROM users "
        "WHERE username = ? AND active = 1 LIMIT 1;";

    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK) {
        throw std::runtime_error(
            "Failed to retrieve user ID: " +
            std::string(sqlite3_errmsg(db))
        );
    }

    sqlite3_bind_text(
        statement, 1, username.c_str(), -1, SQLITE_TRANSIENT
    );

    int result = sqlite3_step(statement);
    std::optional<UserId> userId;

    if (result == SQLITE_ROW) {
        userId = sqlite3_column_int(statement, 0);
    } else if (result != SQLITE_DONE) {
        std::string error = sqlite3_errmsg(db);
        sqlite3_finalize(statement);
        throw std::runtime_error("Failed to retrieve user ID: " + error);
    }

    sqlite3_finalize(statement);
    return userId;
}