
#include "sqlite_course_repository.h"

#include <sqlite3.h>
#include <stdexcept>
#include <string>

SQLiteCourseRepository::SQLiteCourseRepository(
    SQLiteConnection& connection
) : SQLiteRepository(connection) {
}

void SQLiteCourseRepository::initializeTable() {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS courses ("
        "id INTEGER PRIMARY KEY, "
        "department_id INTEGER NOT NULL, "
        "code TEXT NOT NULL UNIQUE, "
        "title TEXT NOT NULL, "
        "FOREIGN KEY(department_id) REFERENCES departments(id)"
        ");";

    connection.execute(sql);
}

bool SQLiteCourseRepository::addCourse(const Course& course) {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    // Check that the department exists.
    const char* checkSql =
        "SELECT 1 FROM departments WHERE id = ?;";

    if (sqlite3_prepare_v2(
        db, checkSql, -1, &statement, nullptr
    ) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    sqlite3_bind_int(statement, 1, course.getDepartmentId());

    int result = sqlite3_step(statement);
    sqlite3_finalize(statement);

    if (result != SQLITE_ROW) {
        return false;
    }

    // Insert the course.
    const char* insertSql =
        "INSERT INTO courses (id, department_id, code, title) "
        "VALUES (?, ?, ?, ?);";

    if (sqlite3_prepare_v2(
        db, insertSql, -1, &statement, nullptr
    ) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    std::string code = course.getCode();
    std::string title = course.getTitle();

    sqlite3_bind_int(statement, 1, course.getId());
    sqlite3_bind_int(statement, 2, course.getDepartmentId());

    sqlite3_bind_text(
        statement, 3, code.c_str(), -1, SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        statement, 4, title.c_str(), -1, SQLITE_TRANSIENT
    );

    result = sqlite3_step(statement);
    sqlite3_finalize(statement);

    return result == SQLITE_DONE;
}

std::vector<Course> SQLiteCourseRepository::getAllCourses() {
    std::vector<Course> courses;

    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "SELECT id, department_id, code, title "
        "FROM courses ORDER BY id;";

    if (sqlite3_prepare_v2(
        db, sql, -1, &statement, nullptr
    ) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    while (sqlite3_step(statement) == SQLITE_ROW) {
        int id = sqlite3_column_int(statement, 0);
        int departmentId = sqlite3_column_int(statement, 1);

        const char* codeText =
            reinterpret_cast<const char*>(
                sqlite3_column_text(statement, 2)
            );

        const char* titleText =
            reinterpret_cast<const char*>(
                sqlite3_column_text(statement, 3)
            );

        courses.emplace_back(
            id,
            departmentId,
            codeText != nullptr ? codeText : "",
            titleText != nullptr ? titleText : ""
        );
    }

    sqlite3_finalize(statement);

    return courses;
}
