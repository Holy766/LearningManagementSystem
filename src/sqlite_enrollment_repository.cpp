
#include "sqlite_enrollment_repository.h"
#include <sqlite3.h>
#include <stdexcept>

SQLiteEnrollmentRepository::SQLiteEnrollmentRepository(
    SQLiteConnection& connection
) : SQLiteRepository(connection) {}

void SQLiteEnrollmentRepository::initializeTable() {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS enrollments ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "student_id INTEGER NOT NULL, "
        "course_id INTEGER NOT NULL, "
        "enrolled_at TEXT NOT NULL, "
        "UNIQUE(student_id, course_id), "
        "FOREIGN KEY(student_id) REFERENCES users(id), "
        "FOREIGN KEY(course_id) REFERENCES courses(id)"
        ");";

    connection.execute(sql);
}

bool SQLiteEnrollmentRepository::enrollStudent(
    int studentId, int courseId
) {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    // Check that the user is an active student.
    const char* studentSql =
        "SELECT 1 FROM users "
        "WHERE id = ? AND role = 'Student' AND active = 1;";

    if (sqlite3_prepare_v2(db, studentSql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    sqlite3_bind_int(statement, 1, studentId);
    int result = sqlite3_step(statement);
    sqlite3_finalize(statement);

    if (result != SQLITE_ROW)
        return false;

    // Check that the course exists.
    const char* courseSql =
        "SELECT 1 FROM courses WHERE id = ?;";

    if (sqlite3_prepare_v2(db, courseSql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    sqlite3_bind_int(statement, 1, courseId);
    result = sqlite3_step(statement);
    sqlite3_finalize(statement);

    if (result != SQLITE_ROW)
        return false;

    // Save the enrollment date.
    const char* insertSql =
        "INSERT INTO enrollments (student_id, course_id, enrolled_at) "
        "VALUES (?, ?, datetime('now'));";

    if (sqlite3_prepare_v2(db, insertSql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    sqlite3_bind_int(statement, 1, studentId);
    sqlite3_bind_int(statement, 2, courseId);

    result = sqlite3_step(statement);
    sqlite3_finalize(statement);

    return result == SQLITE_DONE;
}


std::vector<Enrollment>
SQLiteEnrollmentRepository::getAllEnrollments() {
    std::vector<Enrollment> enrollments;

    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "SELECT id, student_id, course_id "
        "FROM enrollments ORDER BY id;";

    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    while (sqlite3_step(statement) == SQLITE_ROW) {
        int id = sqlite3_column_int(statement, 0);
        int studentId = sqlite3_column_int(statement, 1);
        int courseId = sqlite3_column_int(statement, 2);

        Datetime enrolledAt;

        enrollments.emplace_back(
            id, studentId, courseId, enrolledAt
        );
    }

    sqlite3_finalize(statement);
    return enrollments;
}

bool SQLiteEnrollmentRepository::isStudentEnrolled(
    int studentId,
    int courseId
) {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "SELECT 1 FROM enrollments "
        "WHERE student_id = ? AND course_id = ? "
        "LIMIT 1;";

    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr)
        != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    sqlite3_bind_int(statement, 1, studentId);
    sqlite3_bind_int(statement, 2, courseId);

    int result = sqlite3_step(statement);

    sqlite3_finalize(statement);

    if (result == SQLITE_ROW) {
        return true;
    }

    if (result == SQLITE_DONE) {
        return false;
    }

    throw std::runtime_error(sqlite3_errmsg(db));
}