#ifndef IMARK_AUDIT_LOG_H
#define IMARK_AUDIT_LOG_H

#include "mark_audit_entry.h"
#include <vector>

class IMarkAuditLog {
public:
    virtual ~IMarkAuditLog() = default;

    virtual bool save(const MarkAuditEntry& entry) = 0;

    virtual std::vector<MarkAuditEntry> findAll() = 0;
};

#endif
