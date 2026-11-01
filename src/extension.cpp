
#include "extension.h"
#include <stdexcept>

Extension::Extension(ExtensionId id, AssignmentId assignmentId,UserId studentId, Datetime revisedDeadline,
          string reason, UserId grantedBy)
    : id(id),
      assignmentId(assignmentId),
      studentId(studentId),
      revisedDeadline(revisedDeadline),
      reason(reason),
      grantedBy(grantedBy) {

    if (id <= 0 || assignmentId <= 0 ||
        studentId <= 0 || grantedBy <= 0) {
        throw std::invalid_argument(
            "Extension IDs must be positive."
        );
    }

    if (reason.empty()) {
        throw std::invalid_argument(
            "An extension reason is required."
        );
    }
}


ExtensionId Extension::getId() const {
    return id;
}

AssignmentId Extension::getAssignmentId() const {
    return assignmentId;
}

UserId Extension::getStudentId() const {
    return studentId;
}

const Datetime& Extension::getRevisedDeadline() const {
    return revisedDeadline;
}

const string& Extension::getReason() const {
    return reason;
}

UserId Extension::getGrantedBy() const {
    return grantedBy;
}
