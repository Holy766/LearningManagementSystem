
#include "sqlite_submission_repository.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <cstdio>

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
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);
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

// Bind mark value
sqlite3_bind_double(
    statement, 7, submission.getStoredMark()
);

// Bind feedback
std::string feedback = submission.getStoredFeedback();

sqlite3_bind_text(
    statement, 8, feedback.c_str(), -1, SQLITE_TRANSIENT
);

// Bind has_mark
sqlite3_bind_int(
    statement, 9, submission.hasBeenMarked() ? 1 : 0
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
            state = ?,
            mark_value = ?,
            feedback = ?,
            has_mark = ?
        WHERE id = ?;
    )";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(
            getConnection(), sql, -1, &statement, nullptr
        ) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(getConnection()));
    }

    // 1. Assignment ID
    sqlite3_bind_int(statement, 1, submission.getAssignmentId());

    // 2. Student ID
    sqlite3_bind_int(statement, 2, submission.getStudentId());

    // 3. Submission date
    std::string date = formatDateTime(submission.getSubmittedAt());
    sqlite3_bind_text(statement, 3, date.c_str(), -1, SQLITE_TRANSIENT);

    // 4. File reference
    std::string file = submission.getFileReference();
    sqlite3_bind_text(statement, 4, file.c_str(), -1, SQLITE_TRANSIENT);

    // 5. Submission state
    sqlite3_bind_int(statement, 5, static_cast<int>(submission.getState()));

    // 6. Mark
    sqlite3_bind_double(statement, 6, submission.getStoredMark());

    // 7. Feedback
    std::string feedback = submission.getStoredFeedback();
    sqlite3_bind_text(statement, 7, feedback.c_str(), -1, SQLITE_TRANSIENT);

    // 8. Has mark
    sqlite3_bind_int(statement, 8, submission.hasBeenMarked() ? 1 : 0);

    // 9. Submission ID (WHERE condition)
    sqlite3_bind_int(statement, 9, submission.getId());

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

std::vector<Submission>
SQLiteSubmissionRepository::findByAssignmentId(
    AssignmentId assignmentId
) {
    const char* sql = R"(
        SELECT id, assignment_id, student_id, submitted_at,
               file_reference, state, mark_value, feedback, has_mark
        FROM submissions
        WHERE assignment_id = ?;
    )";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(
            getConnection(), sql, -1, &statement, nullptr
        ) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(getConnection()));
    }

    sqlite3_bind_int(statement, 1, assignmentId);

    std::vector<Submission> submissions;

    int result;

    while ((result = sqlite3_step(statement)) == SQLITE_ROW) {

        SubmissionId id = sqlite3_column_int(statement, 0);
        AssignmentId courseAssignmentId = sqlite3_column_int(statement, 1);
        UserId studentId = sqlite3_column_int(statement, 2);

        std::string dateText =
            reinterpret_cast<const char*>(
                sqlite3_column_text(statement, 3)
            );

        int year, month, day, hours, minutes, seconds;

        if (std::sscanf(dateText.c_str(),"%d-%d-%d %d:%d:%d",&year, &month, &day, &hours, &minutes, &seconds) != 6) {
            sqlite3_finalize(statement);
            throw std::runtime_error("Invalid submission date.");
        }

        Datetime submittedAt;
        submittedAt.setyear(year);
        submittedAt.setmonth(month);
        submittedAt.setday(day);
        submittedAt.sethours(hours);
        submittedAt.setminutes(minutes);
        submittedAt.setseconds(seconds);

        std::string fileReference =
            reinterpret_cast<const char*>(
                sqlite3_column_text(statement, 4)
            );

        SubmissionState state = static_cast<SubmissionState>(
            sqlite3_column_int(statement, 5)
        );

        double markValue = sqlite3_column_double(statement, 6);

        const unsigned char* feedbackText =
            sqlite3_column_text(statement, 7);

        std::string feedback = feedbackText
            ? reinterpret_cast<const char*>(feedbackText)
            : "";

        bool hasMark = sqlite3_column_int(statement, 8) != 0;

        Submission submission(
            id,
            courseAssignmentId,
            studentId,
            submittedAt,
            fileReference,
            state,
            markValue,
            feedback,
            hasMark
        );

        submissions.push_back(submission);
    }

    if (result != SQLITE_DONE) {
        std::string error = sqlite3_errmsg(getConnection());
        sqlite3_finalize(statement);
        throw std::runtime_error(error);
    }

    sqlite3_finalize(statement);

    return submissions;
}

SubmissionId SQLiteSubmissionRepository::getNextSubmissionId() {
    const char* sql =
        "SELECT COALESCE(MAX(id), 0) + 1 FROM submissions;";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(
            getConnection(), sql, -1, &statement, nullptr
        ) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(getConnection()));
    }

    int result = sqlite3_step(statement);

    if (result != SQLITE_ROW) {
        std::string error = sqlite3_errmsg(getConnection());
        sqlite3_finalize(statement);
        throw std::runtime_error(error);
    }

    SubmissionId nextId = sqlite3_column_int(statement, 0);

    sqlite3_finalize(statement);

    return nextId;
}