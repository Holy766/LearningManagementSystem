
#include "mark_audit_entry.h"

MarkAuditEntry::MarkAuditEntry(
    AuditId id, SubmissionId submissionId,
    UserId changedBy, const Datetime& changedAt,
    double oldMark, double newMark, const string& reason)
    : id(id),
      submissionId(submissionId),
      changedBy(changedBy),
      changedAt(changedAt),
      oldMark(oldMark),
      newMark(newMark),
      reason(reason) {
}

AuditId MarkAuditEntry::getId() const {
    return id;
}

SubmissionId MarkAuditEntry::getSubmissionId() const {
    return submissionId;
}

UserId MarkAuditEntry::getChangedBy() const {
    return changedBy;
}

Datetime MarkAuditEntry::getChangedAt() const {
    return changedAt;
}

double MarkAuditEntry::getOldMark() const {
    return oldMark;
}

double MarkAuditEntry::getNewMark() const {
    return newMark;
}

string MarkAuditEntry::getReason() const {
    return reason;
}
