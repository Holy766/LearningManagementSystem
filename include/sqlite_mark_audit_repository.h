#ifndef SQLITE_MARK_AUDIT_REPOSITORY_H
#define SQLITE_MARK_AUDIT_REPOSITORY_H

#include "sqlite_repository.h"
#include "mark_audit_entry.h"
#include "imark_audit_log.h"

class SQLiteMarkAuditRepository : public SQLiteRepository, public IMarkAuditLog {
public:
    explicit SQLiteMarkAuditRepository(SQLiteConnection& connection);

    void initializeTable();

    bool save(const MarkAuditEntry& entry);
};

#endif
