#include "mark_audit_manager.h"

void MarkAuditManager::addEntry(const MarkAuditEntry& entry) {
    entries.push_back(entry);
}

const vector<MarkAuditEntry>& MarkAuditManager::getEntries() const {
    return entries;
}