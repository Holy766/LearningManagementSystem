#ifndef SUBMISSION_H
#define SUBMISSION_H

#include <string>
#include <vector>
#include "mark_audit_entry.h"
#include "mark_audit_manager.h"
#include "user.h"
#include "datetime.h"
#include "ids.h"

using std::vector;
using std::string;
class Assignment;

enum class SubmissionState {
    SUBMITTED,
    LATE,
    MARKED,
    MODERATED,
    RELEASED
};

class Submission {
private:
    SubmissionId id;
    AssignmentId assignmentId;
    UserId studentId;
    Datetime submittedAt;
    string fileReference;
    SubmissionState state;
    double markValue;
    string feedback;
    bool hasMark;

public:
    Submission(SubmissionId id, AssignmentId assignmentId,UserId studentId);

    void submit(string fileReference, Datetime at,const Assignment& assignment);
    bool resubmit(string fileReference, Datetime at,const Assignment& assignment);
    void release();
    void transition(SubmissionState newState);

    void mark(double mark, string feedback, UserId lecturerId,Datetime changedAt, string reason,
         MarkAuditManager& auditManager,const Assignment& assignment);
    double getMark() const;
    string getFeedback() const;
    bool hasBeenMarked() const;
    bool hasSubmitted() const;
    AssignmentId getAssignmentId() const;
    UserId getStudentId() const;
    
    SubmissionId getId() const;
    Datetime getSubmittedAt() const;
    string getFileReference() const;
    SubmissionState getState() const;
    Submission(SubmissionId id,AssignmentId assignmentId,UserId studentId,Datetime submittedAt,
    string fileReference,SubmissionState state,double markValue,string feedback,bool hasMark);
    double getStoredMark() const;
    string getStoredFeedback() const;

};

#endif
