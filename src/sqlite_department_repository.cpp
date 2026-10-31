
#include "sqlite_department_repository.h"

#include <sqlite3.h>
#include <stdexcept>
#include <string>

SQLiteDepartmentRepository::SQLiteDepartmentRepository(
    SQLiteConnection& connection
) : SQLiteRepository(connection) {
}

void SQLiteDepartmentRepository::initializeTable() {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS departments ("
        "id INTEGER PRIMARY KEY, "
        "name TEXT NOT NULL UNIQUE, "
        "hod_id INTEGER NOT NULL, "
        "FOREIGN KEY(hod_id) REFERENCES users(id)"
        ");";

    connection.execute(sql);
}

bool SQLiteDepartmentRepository::addDepartment(
    const Department& department
) {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    // Check that the selected user is actually a HoD.
    const char* checkSql =
        "SELECT 1 FROM users "
        "WHERE id = ? AND role = 'HoD' AND active = 1;";

    if (sqlite3_prepare_v2(
        db, checkSql, -1, &statement, nullptr
    ) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    sqlite3_bind_int(statement, 1, department.getHodId());

    int result = sqlite3_step(statement);

    sqlite3_finalize(statement);

    if (result != SQLITE_ROW) {
        return false;
    }

    // Insert the department.
    const char* insertSql =
        "INSERT INTO departments (id, name, hod_id) "
        "VALUES (?, ?, ?);";

    if (sqlite3_prepare_v2(
        db, insertSql, -1, &statement, nullptr
    ) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    std::string name = department.getName();

    sqlite3_bind_int(statement, 1, department.getId());
    sqlite3_bind_text(
        statement, 2, name.c_str(), -1, SQLITE_TRANSIENT
    );
    sqlite3_bind_int(statement, 3, department.getHodId());

    result = sqlite3_step(statement);

    sqlite3_finalize(statement);

    return result == SQLITE_DONE;
}

std::vector<Department>
SQLiteDepartmentRepository::getAllDepartments() {
    std::vector<Department> departments;

    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "SELECT id, name, hod_id FROM departments ORDER BY id;";

    if (sqlite3_prepare_v2(
        db, sql, -1, &statement, nullptr
    ) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    while (sqlite3_step(statement) == SQLITE_ROW) {
        int id = sqlite3_column_int(statement, 0);

        const char* nameText =
            reinterpret_cast<const char*>(
                sqlite3_column_text(statement, 1)
            );

        int hodId = sqlite3_column_int(statement, 2);

        departments.emplace_back(
            id,
            nameText != nullptr ? nameText : "",
            hodId
        );
    }

    sqlite3_finalize(statement);

    return departments;
}
