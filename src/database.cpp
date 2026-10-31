#include "database.h"
#include <stdexcept>

SQLiteConnection::SQLiteConnection(const std::string& dataPath)
    : dataPath(dataPath), db(nullptr) {
    open();
}

SQLiteConnection::~SQLiteConnection() {
    if (db != nullptr) {
        sqlite3_close(db);
    }
}

void SQLiteConnection::open() {
    if (db != nullptr) {
        return;
    }

    int result = sqlite3_open(dataPath.c_str(), &db);

    if (result != SQLITE_OK) {
        std::string error =
            db != nullptr ? sqlite3_errmsg(db) : "Unknown error";

        if (db != nullptr) {
            sqlite3_close(db);
            db = nullptr;
        }

        throw std::runtime_error(
            "Database connection failed: " + error
        );
    }
}

void SQLiteConnection::close() {
    if (db == nullptr) {
        return;
    }

    int result = sqlite3_close(db);

    if (result != SQLITE_OK) {
        throw std::runtime_error(
            "Could not close database: " +
            std::string(sqlite3_errmsg(db))
        );
    }

    db = nullptr;
}

void SQLiteConnection::execute(const std::string& sql) {
    if (db == nullptr) {
        throw std::runtime_error("Database is not open.");
    }

    char* errorMessage = nullptr;

    int result = sqlite3_exec(
        db,
        sql.c_str(),
        nullptr,
        nullptr,
        &errorMessage
    );

    if (result != SQLITE_OK) {
        std::string error =
            errorMessage != nullptr
                ? errorMessage
                : "Unknown error";

        sqlite3_free(errorMessage);

        throw std::runtime_error(
            "SQL execution failed: " + error
        );
    }
}

sqlite3* SQLiteConnection::getConnection() const {
    return db;
}
