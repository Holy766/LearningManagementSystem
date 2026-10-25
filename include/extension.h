
#ifndef EXTENSION_H
#define EXTENSION_H

#include <string>
#include "user.h"
#include "datetime.h"
#include "ids.h"

using std::string;


class Extension {
private:
    ExtensionId id;
    AssignmentId assignmentId;
    UserId studentId;
    Datetime revisedDeadline;
    string reason;
    UserId grantedBy;

public:
    Extension(ExtensionId id, AssignmentId assignmentId,
              UserId studentId, Datetime revisedDeadline,
              string reason, UserId grantedBy);

    ExtensionId getId() const;
    AssignmentId getAssignmentId() const;
    UserId getStudentId() const;
    Datetime getRevisedDeadline() const;
    string getReason() const;
    UserId getGrantedBy() const;
};

#endif
