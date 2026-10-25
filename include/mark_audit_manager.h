#ifndef MARK_AUDIT_MANAGER_H
#define MARK_AUDIT_MANAGER_H

#include <vector>
#include "mark_audit_entry.h"

using std::vector;

class MarkAuditManager {
private:
    vector<MarkAuditEntry> entries;

public:
    void addEntry(const MarkAuditEntry& entry);
    const vector<MarkAuditEntry>& getEntries() const;
};

#endif