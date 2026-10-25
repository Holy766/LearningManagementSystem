
#include "sqlite_submission_repository.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>

static std::string formatDateTime(Datetime date) {
    std::ostringstream output;

    output << date.getyear() << "-"
           << std::setw(2) << std::setfill('0') << date.getmonth() << "-"
           << std::setw(2) << std::setfill('0') << date.getday() << " "
           << std::setw(2) << std::setfill('0') << date.gethours() << ":"
           << std::setw(2) << std::setfill('0') << date.getminutes() << ":"
           << std::setw(2) << std::setfill('0') << date.getseconds();

    return output.str();
}

SQLiteSubmissionRepository::SQLiteSubmissionRepository(
    SQLiteConnection& connection
) : SQLiteRepository(connection) {
}

void SQLiteSubmissionRepository::initializeTable() {
    const std::string sql = R"(
        CREATE TABLE IF NOT EXISTS submissions (
            id INTEGER PRIMARY KEY,
            assignment_id INTEGER NOT NULL,
            student_id INTEGER NOT NULL,
            submitted_at TEXT NOT NULL,
            file_reference TEXT NOT NULL,
            state INTEGER NOT NULL,
            mark_value REAL,
            feedback TEXT,
            has_mark INTEGER NOT NULL DEFAULT 0
        );
    )";

    connection.execute(sql);
}

bool SQLiteSubmissionRepository::save(const Submission& submission) {
    const char* sql = R"(
        INSERT INTO submissions (
            id, assignment_id, student_id, submitted_at,
            file_reference, state, mark_value, feedback, has_mark
        )
        VALUES (?, ?, ?, ?, ?, ?, NULL, NULL, 0);
    )";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(
            getConnection(), sql, -1, &statement, nullptr
        ) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(getConnection()));
    }

    sqlite3_bind_int(statement, 1, submission.getId());
    sqlite3_bind_int(statement, 2, submission.getAssignmentId());
    sqlite3_bind_int(statement, 3, submission.getStudentId());

    std::string date = formatDateTime(submission.getSubmittedAt());
    sqlite3_bind_text(statement, 4, date.c_str(), -1, SQLITE_TRANSIENT);

    std::string file = submission.getFileReference();
    sqlite3_bind_text(statement, 5, file.c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_bind_int(
        statement, 6, static_cast<int>(submission.getState())
    );

    int result = sqlite3_step(statement);
    sqlite3_finalize(statement);

    if (result != SQLITE_DONE) {
        throw std::runtime_error(sqlite3_errmsg(getConnection()));
    }

    return true;
}


std::optional<Submission> SQLiteSubmissionRepository::findById(
    SubmissionId id
) {
    const char* sql = R"(
        SELECT id, assignment_id, student_id
        FROM submissions
        WHERE id = ?;
    )";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(
            getConnection(), sql, -1, &statement, nullptr
        ) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(getConnection()));
    }

    sqlite3_bind_int(statement, 1, id);

    int result = sqlite3_step(statement);

    if (result == SQLITE_DONE) {
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    if (result != SQLITE_ROW) {
        std::string error = sqlite3_errmsg(getConnection());
        sqlite3_finalize(statement);
        throw std::runtime_error(error);
    }

    Submission submission(
        sqlite3_column_int(statement, 0),
        sqlite3_column_int(statement, 1),
        sqlite3_column_int(statement, 2)
    );

    sqlite3_finalize(statement);
    return submission;
}


bool SQLiteSubmissionRepository::update(const Submission& submission) {
    const char* sql = R"(
        UPDATE submissions
        SET assignment_id = ?,
            student_id = ?,
            submitted_at = ?,
            file_reference = ?,
            state = ?
        WHERE id = ?;
    )";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(
            getConnection(), sql, -1, &statement, nullptr
        ) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(getConnection()));
    }

    sqlite3_bind_int(statement, 1, submission.getAssignmentId());
    sqlite3_bind_int(statement, 2, submission.getStudentId());

    std::string date = formatDateTime(submission.getSubmittedAt());
    sqlite3_bind_text(statement, 3, date.c_str(), -1, SQLITE_TRANSIENT);

    std::string file = submission.getFileReference();
    sqlite3_bind_text(statement, 4, file.c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_bind_int(
        statement, 5, static_cast<int>(submission.getState())
    );

    sqlite3_bind_int(statement, 6, submission.getId());

    int result = sqlite3_step(statement);

    if (result != SQLITE_DONE) {
        std::string error = sqlite3_errmsg(getConnection());
        sqlite3_finalize(statement);
        throw std::runtime_error(error);
    }

    int rowsChanged = sqlite3_changes(getConnection());
    sqlite3_finalize(statement);

    return rowsChanged > 0;
}


bool SQLiteSubmissionRepository::remove(SubmissionId id) {
    const char* sql = "DELETE FROM submissions WHERE id = ?;";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(
            getConnection(), sql, -1, &statement, nullptr
        ) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(getConnection()));
    }

    sqlite3_bind_int(statement, 1, id);

    int result = sqlite3_step(statement);

    if (result != SQLITE_DONE) {
        std::string error = sqlite3_errmsg(getConnection());
        sqlite3_finalize(statement);
        throw std::runtime_error(error);
    }

    int rowsChanged = sqlite3_changes(getConnection());

    sqlite3_finalize(statement);

    return rowsChanged > 0;
}
