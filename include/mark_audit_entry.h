
#ifndef MARK_AUDIT_ENTRY_H
#define MARK_AUDIT_ENTRY_H

#include <string>
#include "user.h"
#include "datetime.h"
#include "ids.h"

using std::string;
using AuditId = int;

class MarkAuditEntry {
private:
    AuditId id;
    SubmissionId submissionId;
    UserId changedBy;
    Datetime changedAt;
    double oldMark;
    double newMark;
    string reason;

public:
    MarkAuditEntry(AuditId id, SubmissionId submissionId, UserId changedBy,const Datetime& changedAt,
        double oldMark, double newMark,const string& reason);

    AuditId getId() const;
    SubmissionId getSubmissionId() const;
    UserId getChangedBy() const;
    Datetime getChangedAt() const;
    double getOldMark() const;
    double getNewMark() const;
    string getReason() const;
};

#endif
