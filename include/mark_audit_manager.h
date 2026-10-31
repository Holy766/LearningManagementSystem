#ifndef MARK_AUDIT_MANAGER_H
#define MARK_AUDIT_MANAGER_H

#include <vector>
#include "mark_audit_entry.h"
#include "imark_audit_log.h"

using std::vector;

class MarkAuditManager {
private:
    vector<MarkAuditEntry> entries;
    IMarkAuditLog& auditLog;

public:
    explicit MarkAuditManager(IMarkAuditLog& auditLog);

    void addEntry(const MarkAuditEntry& entry);
    const vector<MarkAuditEntry>& getEntries() const;
};

#endif