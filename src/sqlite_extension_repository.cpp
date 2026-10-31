#include "sqlite_extension_repository.h"
#include <sqlite3.h>
#include <stdexcept>
#include <string>

SQLiteExtensionRepository::SQLiteExtensionRepository(
    SQLiteConnection& connection
) : SQLiteRepository(connection) {
}

void SQLiteExtensionRepository::initializeTable() {
    sqlite3* db = getConnection();

    const char* sql =
        "CREATE TABLE IF NOT EXISTS extensions ("
        "id INTEGER PRIMARY KEY, "
        "assignment_id INTEGER NOT NULL, "
        "student_id INTEGER NOT NULL, "
        "day INTEGER NOT NULL, "
        "month INTEGER NOT NULL, "
        "year INTEGER NOT NULL, "
        "hours INTEGER NOT NULL, "
        "minutes INTEGER NOT NULL, "
        "seconds INTEGER NOT NULL, "
        "reason TEXT NOT NULL, "
        "granted_by INTEGER NOT NULL"
        ");";

    char* errorMessage = nullptr;

    if (sqlite3_exec(db, sql, nullptr, nullptr, &errorMessage)
        != SQLITE_OK) {

        std::string error = errorMessage;
        sqlite3_free(errorMessage);
        throw std::runtime_error(error);
    }
}

bool SQLiteExtensionRepository::save(const Extension& extension) {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "INSERT INTO extensions "
        "(id, assignment_id, student_id, day, month, year, "
        "hours, minutes, seconds, reason, granted_by) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr)
        != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    Datetime deadline = extension.getRevisedDeadline();

    sqlite3_bind_int(statement, 1, extension.getId());
    sqlite3_bind_int(statement, 2, extension.getAssignmentId());
    sqlite3_bind_int(statement, 3, extension.getStudentId());
    sqlite3_bind_int(statement, 4, deadline.getday());
    sqlite3_bind_int(statement, 5, deadline.getmonth());
    sqlite3_bind_int(statement, 6, deadline.getyear());
    sqlite3_bind_int(statement, 7, deadline.gethours());
    sqlite3_bind_int(statement, 8, deadline.getminutes());
    sqlite3_bind_int(statement, 9, deadline.getseconds());

    sqlite3_bind_text(
        statement, 10, extension.getReason().c_str(),
        -1, SQLITE_TRANSIENT
    );

    sqlite3_bind_int(statement, 11, extension.getGrantedBy());

    int result = sqlite3_step(statement);
    sqlite3_finalize(statement);

    return result == SQLITE_DONE;
}
std::optional<Extension> SQLiteExtensionRepository::findById(
    ExtensionId id
) {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "SELECT assignment_id, student_id, day, month, year, "
        "hours, minutes, seconds, reason, granted_by "
        "FROM extensions WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr)
        != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    sqlite3_bind_int(statement, 1, id);

    int result = sqlite3_step(statement);

    if (result == SQLITE_DONE) {
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    if (result != SQLITE_ROW) {
        std::string error = sqlite3_errmsg(db);
        sqlite3_finalize(statement);
        throw std::runtime_error(error);
    }

    int assignmentId = sqlite3_column_int(statement, 0);
    int studentId = sqlite3_column_int(statement, 1);

    Datetime deadline;
    deadline.setday(sqlite3_column_int(statement, 2));
    deadline.setmonth(sqlite3_column_int(statement, 3));
    deadline.setyear(sqlite3_column_int(statement, 4));
    deadline.sethours(sqlite3_column_int(statement, 5));
    deadline.setminutes(sqlite3_column_int(statement, 6));
    deadline.setseconds(sqlite3_column_int(statement, 7));

    string reason = reinterpret_cast<const char*>(
        sqlite3_column_text(statement, 8)
    );

    int grantedBy = sqlite3_column_int(statement, 9);

    sqlite3_finalize(statement);

    return Extension(
        id, assignmentId, studentId,
        deadline, reason, grantedBy
    );
}

std::vector<Extension>
SQLiteExtensionRepository::findByAssignmentAndStudent(
    AssignmentId assignmentId,
    UserId studentId
) {
    sqlite3* db = getConnection();
    sqlite3_stmt* statement = nullptr;

    const char* sql =
        "SELECT id, day, month, year, hours, minutes, "
        "seconds, reason, granted_by "
        "FROM extensions "
        "WHERE assignment_id = ? AND student_id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr)
        != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    sqlite3_bind_int(statement, 1, assignmentId);
    sqlite3_bind_int(statement, 2, studentId);

    std::vector<Extension> extensions;

    int result;

    while ((result = sqlite3_step(statement)) == SQLITE_ROW) {

        ExtensionId id = sqlite3_column_int(statement, 0);

        Datetime deadline;
        deadline.setday(sqlite3_column_int(statement, 1));
        deadline.setmonth(sqlite3_column_int(statement, 2));
        deadline.setyear(sqlite3_column_int(statement, 3));
        deadline.sethours(sqlite3_column_int(statement, 4));
        deadline.setminutes(sqlite3_column_int(statement, 5));
        deadline.setseconds(sqlite3_column_int(statement, 6));

        const unsigned char* text =
            sqlite3_column_text(statement, 7);

        std::string reason = text
            ? reinterpret_cast<const char*>(text)
            : "";

        UserId grantedBy = sqlite3_column_int(statement, 8);

        extensions.emplace_back(
            id,
            assignmentId,
            studentId,
            deadline,
            reason,
            grantedBy
        );
    }

    if (result != SQLITE_DONE) {
        std::string error = sqlite3_errmsg(db);
        sqlite3_finalize(statement);
        throw std::runtime_error(error);
    }

    sqlite3_finalize(statement);

    return extensions;
}