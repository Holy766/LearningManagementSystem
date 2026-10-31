
#include "sqlite_lecturer_assignment_repository.h"
#include <sqlite3.h>
#include <stdexcept>

SQLiteLecturerAssignmentRepository::SQLiteLecturerAssignmentRepository(
    SQLiteConnection& connection
) : SQLiteRepository(connection) {}

void SQLiteLecturerAssignmentRepository::initializeTable() {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS lecturer_assignments ("
        "course_id INTEGER PRIMARY KEY, "
        "lecturer_id INTEGER NOT NULL, "
        "FOREIGN KEY(course_id) REFERENCES courses(id), "
        "FOREIGN KEY(lecturer_id) REFERENCES users(id)"
        ");";

    connection.execute(sql);
}

bool SQLiteLecturerAssignmentRepository::assignLecturer(
    int courseId, int lecturerId
) {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    // Check that the course exists.
    const char* courseSql =
        "SELECT 1 FROM courses WHERE id = ?;";

    if (sqlite3_prepare_v2(db, courseSql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    sqlite3_bind_int(statement, 1, courseId);
    int result = sqlite3_step(statement);
    sqlite3_finalize(statement);

    if (result != SQLITE_ROW)
        return false;

    // Check that the user exists, is a lecturer, and is active.
    const char* lecturerSql =
        "SELECT 1 FROM users "
        "WHERE id = ? AND role = 'Lecturer' AND active = 1;";

    if (sqlite3_prepare_v2(db, lecturerSql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    sqlite3_bind_int(statement, 1, lecturerId);
    result = sqlite3_step(statement);
    sqlite3_finalize(statement);

    if (result != SQLITE_ROW)
        return false;

    // Insert the assignment.
    const char* insertSql =
        "INSERT INTO lecturer_assignments (course_id, lecturer_id) "
        "VALUES (?, ?);";

    if (sqlite3_prepare_v2(db, insertSql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    sqlite3_bind_int(statement, 1, courseId);
    sqlite3_bind_int(statement, 2, lecturerId);

    result = sqlite3_step(statement);
    sqlite3_finalize(statement);

    return result == SQLITE_DONE;
}

int SQLiteLecturerAssignmentRepository::getLecturerForCourse(int courseId) {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "SELECT lecturer_id FROM lecturer_assignments "
        "WHERE course_id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    sqlite3_bind_int(statement, 1, courseId);

    int lecturerId = -1;

    if (sqlite3_step(statement) == SQLITE_ROW)
        lecturerId = sqlite3_column_int(statement, 0);

    sqlite3_finalize(statement);
    return lecturerId;
}

std::vector<int> SQLiteLecturerAssignmentRepository::getCoursesForLecturer(
    int lecturerId
) {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "SELECT course_id FROM lecturer_assignments "
        "WHERE lecturer_id = ? ORDER BY course_id;";

    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    sqlite3_bind_int(statement, 1, lecturerId);

    std::vector<int> courseIds;

    int result;
    while ((result = sqlite3_step(statement)) == SQLITE_ROW) {
        courseIds.push_back(sqlite3_column_int(statement, 0));
    }

    if (result != SQLITE_DONE) {
        std::string error = sqlite3_errmsg(db);
        sqlite3_finalize(statement);
        throw std::runtime_error(error);
    }

    sqlite3_finalize(statement);
    return courseIds;
}