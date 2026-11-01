#include "mark_audit_manager.h"

MarkAuditManager::MarkAuditManager(IMarkAuditLog& auditLog): auditLog(auditLog),entries(auditLog.findAll()) {
}
void MarkAuditManager::addEntry(const MarkAuditEntry& entry) {
    auditLog.save(entry);
    entries.push_back(entry);
}

const vector<MarkAuditEntry>& MarkAuditManager::getEntries() const {
    return entries;
}