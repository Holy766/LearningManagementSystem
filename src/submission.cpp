#include <iostream>
#include <stdexcept>

#include "submission.h"
#include "assignment.h"
#include "mark_audit_manager.h"
#include "mark_audit_entry.h"

using namespace std;

// Constructor
Submission::Submission(SubmissionId id, AssignmentId assignmentId,UserId studentId)
    : id(id),assignmentId(assignmentId), studentId(studentId),submittedAt(),fileReference(""),
      state(SubmissionState::SUBMITTED),markValue(0.0),feedback(""),hasMark(false) {
}
// Constructor for restoring a submission from the database
Submission::Submission(SubmissionId id,AssignmentId assignmentId,UserId studentId,Datetime submittedAt,string fileReference,SubmissionState state,double markValue,
    string feedback,bool hasMark): id(id),assignmentId(assignmentId),studentId(studentId),submittedAt(submittedAt),fileReference(fileReference),
      state(state),markValue(markValue),feedback(feedback),hasMark(hasMark) {
}
// Submit work
void Submission::submit(const string& fileReference, const Datetime& at,
    const Assignment& assignment) {
    if (hasSubmitted()) {
    throw std::invalid_argument(
        "Work has already been submitted. Use resubmit instead."
    );
    }
    Datetime effectiveDeadline =
        assignment.getEffectiveDeadline(studentId);

    if (effectiveDeadline.isBefore(at)) {
        cout << "Submission rejected: deadline has passed."
             << endl;
        return;
    }

    this->fileReference = fileReference;
    submittedAt = at;
    if (at.isBefore(effectiveDeadline)) {
    state = SubmissionState::SUBMITTED;
 } else {
    state = SubmissionState::LATE;
}

    cout << "Submission successful." << endl;
}
// Resubmit work
bool Submission::resubmit(const string& fileReference, const Datetime& at,const Assignment& assignment) {
     if (assignment.getEffectiveDeadline(studentId).isBefore(at)) {
        cout << "Resubmission rejected: deadline has passed."<< endl;
        return false;
    }

    this->fileReference = fileReference;
    submittedAt = at;
    state = SubmissionState::SUBMITTED;

    cout << "Resubmission successful." << endl;
    return true;
}

// Release marks

void Submission::release() {
    if (state != SubmissionState::MODERATED) {
        throw std::logic_error(
            "Only moderated submissions can be released."
        );
    }

    state = SubmissionState::RELEASED;
}

// Change submission state
void Submission::transition(SubmissionState newState) {
    bool validTransition = false;

    if (state == SubmissionState::SUBMITTED ||
        state == SubmissionState::LATE) {
        validTransition = (newState == SubmissionState::MARKED);
    }
    else if (state == SubmissionState::MARKED) {
        validTransition = (newState == SubmissionState::MODERATED);
    }

    if (!validTransition) {
        throw std::logic_error("Invalid submission state transition.");
    }

    state = newState;
}
// Mark submission

void Submission::mark(double mark,const string& feedback, UserId lecturerId,
    const Datetime& changedAt, const string& reason,
    MarkAuditManager& auditManager, const Assignment& assignment) {

    if (mark < 0 || mark > assignment.getMaximumMark()) {
        throw invalid_argument(
            "Mark must be between 0 and the maximum mark."
        );
    }

    double oldMark = hasMark ? markValue : 0.0;
    double finalMark = mark;

    // Calculate the penalty if the submission was accepted as late.
    if (state == SubmissionState::LATE) {
        int daysLate =assignment.getEffectiveDeadline(studentId).daysDifference(submittedAt);
        finalMark = assignment.calculateLatePenalty(mark, daysLate);
    }

    MarkAuditEntry entry(
        0,
        id,
        lecturerId,
        changedAt,
        oldMark,
        finalMark,
        reason
    );

    auditManager.addEntry(entry);

    markValue = finalMark;
    this->feedback = feedback;
    hasMark = true;
    state = SubmissionState::MARKED;

    cout << "Submission marked by lecturer ID: "<< lecturerId << endl;
    cout << "Original mark: " << mark << endl;
    cout << "Final mark after penalty: " << markValue << endl;
    cout << "Feedback: " << this->feedback << endl;
}

double Submission::getMark() const {
    if (state != SubmissionState::MODERATED &&
        state != SubmissionState::RELEASED) {
        throw std::logic_error(
            "Mark is not available until HoD moderation."
        );
    }

    return markValue;
}

string Submission::getFeedback() const {
    if (state != SubmissionState::MODERATED &&
        state != SubmissionState::RELEASED) {
        throw std::logic_error(
            "Feedback is not available until HoD moderation."
        );
    }

    return feedback;
}

bool Submission::hasBeenMarked() const {
    return hasMark;
}
bool Submission::hasSubmitted() const {
    return !fileReference.empty();
}

AssignmentId Submission::getAssignmentId() const {
    return assignmentId;
}

UserId Submission::getStudentId() const {
    return studentId;
}


SubmissionId Submission::getId() const {
    return id;
}

Datetime Submission::getSubmittedAt() const {
    return submittedAt;
}

string Submission::getFileReference() const {
    return fileReference;
}

SubmissionState Submission::getState() const {
    return state;
}

double Submission::getStoredMark() const {
    return markValue;
}

string Submission::getStoredFeedback() const {
    return feedback;
}