
#include "sqlite_assignment_repository.h"
#include <sqlite3.h>
#include <stdexcept>

SQLiteAssignmentRepository::SQLiteAssignmentRepository(
    SQLiteConnection& connection
) : SQLiteRepository(connection) {}

void SQLiteAssignmentRepository::initializeTable() {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS assignments ("
        "id INTEGER PRIMARY KEY, "
        "course_id INTEGER NOT NULL, "
        "title TEXT NOT NULL, "
        "day INTEGER NOT NULL, "
        "month INTEGER NOT NULL, "
        "year INTEGER NOT NULL, "
        "hours INTEGER NOT NULL, "
        "minutes INTEGER NOT NULL, "
        "seconds INTEGER NOT NULL, "
        "maximum_mark REAL NOT NULL, "
        "FOREIGN KEY(course_id) REFERENCES courses(id)"
        ");";

    connection.execute(sql);
}

bool SQLiteAssignmentRepository::save(
    const Assignment& assignment
) {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "INSERT INTO assignments "
        "(id, course_id, title, day, month, year, "
        "hours, minutes, seconds, maximum_mark) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    Datetime deadline = assignment.getDeadline();

    std::string title = assignment.getTitle();

    sqlite3_bind_int(statement, 1, assignment.getId());
    sqlite3_bind_int(statement, 2, assignment.getCourseId());
    sqlite3_bind_text(statement, 3, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(statement, 4, deadline.getday());
    sqlite3_bind_int(statement, 5, deadline.getmonth());
    sqlite3_bind_int(statement, 6, deadline.getyear());
    sqlite3_bind_int(statement, 7, deadline.gethours());
    sqlite3_bind_int(statement, 8, deadline.getminutes());
    sqlite3_bind_int(statement, 9, deadline.getseconds());
    sqlite3_bind_double(statement, 10, assignment.getMaximumMark());

    int result = sqlite3_step(statement);
    sqlite3_finalize(statement);

    return result == SQLITE_DONE;
}

std::optional<Assignment>
SQLiteAssignmentRepository::findById(AssignmentId id) {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "SELECT id, course_id, title, day, month, year, "
        "hours, minutes, seconds, maximum_mark "
        "FROM assignments WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    sqlite3_bind_int(statement, 1, id);

    std::optional<Assignment> assignment = std::nullopt;

    if (sqlite3_step(statement) == SQLITE_ROW) {
        Datetime deadline;

        deadline.setday(sqlite3_column_int(statement, 3));
        deadline.setmonth(sqlite3_column_int(statement, 4));
        deadline.setyear(sqlite3_column_int(statement, 5));
        deadline.sethours(sqlite3_column_int(statement, 6));
        deadline.setminutes(sqlite3_column_int(statement, 7));
        deadline.setseconds(sqlite3_column_int(statement, 8));

        const char* titleText = reinterpret_cast<const char*>(
            sqlite3_column_text(statement, 2)
        );

        assignment.emplace(
            sqlite3_column_int(statement, 0),
            sqlite3_column_int(statement, 1),
            titleText ? titleText : "",
            deadline,
            sqlite3_column_double(statement, 9)
        );
    }

    sqlite3_finalize(statement);
    return assignment;
}

std::vector<Assignment>
SQLiteAssignmentRepository::findByCourseId(CourseId courseId) {
    std::vector<Assignment> assignments;

    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "SELECT id, course_id, title, day, month, year, "
        "hours, minutes, seconds, maximum_mark "
        "FROM assignments WHERE course_id = ? ORDER BY id;";

    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    sqlite3_bind_int(statement, 1, courseId);

    while (sqlite3_step(statement) == SQLITE_ROW) {
        Datetime deadline;

        deadline.setday(sqlite3_column_int(statement, 3));
        deadline.setmonth(sqlite3_column_int(statement, 4));
        deadline.setyear(sqlite3_column_int(statement, 5));
        deadline.sethours(sqlite3_column_int(statement, 6));
        deadline.setminutes(sqlite3_column_int(statement, 7));
        deadline.setseconds(sqlite3_column_int(statement, 8));

        const char* titleText = reinterpret_cast<const char*>(
            sqlite3_column_text(statement, 2)
        );

        assignments.emplace_back(
            sqlite3_column_int(statement, 0),
            sqlite3_column_int(statement, 1),
            titleText ? titleText : "",
            deadline,
            sqlite3_column_double(statement, 9)
        );
    }

    sqlite3_finalize(statement);
    return assignments;
}

bool SQLiteAssignmentRepository::update(
    const Assignment& assignment
) {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "UPDATE assignments SET "
        "title = ?, day = ?, month = ?, year = ?, "
        "hours = ?, minutes = ?, seconds = ?, maximum_mark = ? "
        "WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    Datetime deadline = assignment.getDeadline();
    std::string title = assignment.getTitle();

    sqlite3_bind_text(statement, 1, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(statement, 2, deadline.getday());
    sqlite3_bind_int(statement, 3, deadline.getmonth());
    sqlite3_bind_int(statement, 4, deadline.getyear());
    sqlite3_bind_int(statement, 5, deadline.gethours());
    sqlite3_bind_int(statement, 6, deadline.getminutes());
    sqlite3_bind_int(statement, 7, deadline.getseconds());
    sqlite3_bind_double(statement, 8, assignment.getMaximumMark());
    sqlite3_bind_int(statement, 9, assignment.getId());

    int result = sqlite3_step(statement);
    bool updated = result == SQLITE_DONE && sqlite3_changes(db) > 0;

    sqlite3_finalize(statement);
    return updated;
}

bool SQLiteAssignmentRepository::remove(AssignmentId id) {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql = "DELETE FROM assignments WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    sqlite3_bind_int(statement, 1, id);

    int result = sqlite3_step(statement);
    bool removed = result == SQLITE_DONE && sqlite3_changes(db) > 0;

    sqlite3_finalize(statement);
    return removed;
}
