#include "sqlite_mark_audit_repository.h"
#include <stdexcept>
#include <string>
#include <sstream>
#include <iomanip>

SQLiteMarkAuditRepository::SQLiteMarkAuditRepository(
    SQLiteConnection& connection
) : SQLiteRepository(connection) {
}

void SQLiteMarkAuditRepository::initializeTable() {
    const std::string sql = R"(
        CREATE TABLE IF NOT EXISTS mark_audit (
            id INTEGER PRIMARY KEY,
            submission_id INTEGER NOT NULL,
            changed_by INTEGER NOT NULL,
            changed_at TEXT NOT NULL,
            old_mark REAL NOT NULL,
            new_mark REAL NOT NULL,
            reason TEXT NOT NULL
        );
    )";

    connection.execute(sql);
}

static std::string formatAuditDateTime(Datetime date) {
    std::ostringstream output;

    output << date.getyear() << "-"
           << std::setw(2) << std::setfill('0') << date.getmonth() << "-"
           << std::setw(2) << std::setfill('0') << date.getday() << " "
           << std::setw(2) << std::setfill('0') << date.gethours() << ":"
           << std::setw(2) << std::setfill('0') << date.getminutes() << ":"
           << std::setw(2) << std::setfill('0') << date.getseconds();

    return output.str();
}

bool SQLiteMarkAuditRepository::save(const MarkAuditEntry& entry) {
    const char* sql = R"(
        INSERT INTO mark_audit (
            id, submission_id, changed_by, changed_at,
            old_mark, new_mark, reason
        )
        VALUES (?, ?, ?, ?, ?, ?, ?);
    )";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(
            getConnection(), sql, -1, &statement, nullptr
        ) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(getConnection()));
    }

    sqlite3_bind_int(statement, 1, entry.getId());
    sqlite3_bind_int(statement, 2, entry.getSubmissionId());
    sqlite3_bind_int(statement, 3, entry.getChangedBy());

    std::string date = formatAuditDateTime(entry.getChangedAt());
    sqlite3_bind_text(statement, 4, date.c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_bind_double(statement, 5, entry.getOldMark());
    sqlite3_bind_double(statement, 6, entry.getNewMark());

    std::string reason = entry.getReason();
    sqlite3_bind_text(statement, 7, reason.c_str(), -1, SQLITE_TRANSIENT);

    int result = sqlite3_step(statement);

    if (result != SQLITE_DONE) {
        std::string error = sqlite3_errmsg(getConnection());
        sqlite3_finalize(statement);
        throw std::runtime_error(error);
    }

    sqlite3_finalize(statement);
    return true;
}
