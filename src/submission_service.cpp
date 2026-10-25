#include "submission_service.h"
#include <stdexcept>

bool SubmissionService::submitWork(
    User& student,
    Submission& submission,
    Assignment& assignment,
    const string& fileReference,
    Datetime submittedAt
) {
    if (!student.isActive()) {
        throw std::runtime_error("Inactive users cannot submit work.");
    }

    if (!student.haspermission(Permission::SUBMIT_WORK)) {
        throw std::runtime_error("You do not have permission to submit work.");
    }

    if (submission.getStudentId() != student.getId()) {
        throw std::runtime_error(
            "This submission does not belong to this student."
        );
    }

    if (submission.getAssignmentId() != assignment.getId()) {
        throw std::runtime_error(
            "This submission belongs to a different assignment."
        );
    }

    submission.submit(fileReference, submittedAt, assignment);
    return submission.hasSubmitted();
}

bool SubmissionService::resubmitWork(
    User& student,
    Submission& submission,
    Assignment& assignment,
    const string& fileReference,
    Datetime submittedAt
) {
    if (!student.isActive()) {
        throw std::runtime_error("Inactive users cannot resubmit work.");
    }

    if (!student.haspermission(Permission::SUBMIT_WORK)) {
        throw std::runtime_error("You do not have permission to resubmit work.");
    }

    if (submission.getStudentId() != student.getId()) {
        throw std::runtime_error(
            "This submission does not belong to this student."
        );
    }

    if (submission.getAssignmentId() != assignment.getId()) {
        throw std::runtime_error(
            "This submission belongs to a different assignment."
        );
    }

    return submission.resubmit(fileReference, submittedAt, assignment);
}


bool SubmissionService::moderateSubmission(
    User& hod,
    Submission& submission
) {
    if (!hod.isActive()) {
        throw std::runtime_error(
            "Inactive users cannot moderate submissions."
        );
    }

    if (!hod.haspermission(Permission::MODERATE_MARKS)) {
        throw std::runtime_error(
            "You do not have permission to moderate marks."
        );
    }

    if (submission.getState() != SubmissionState::MARKED) {
        throw std::runtime_error(
            "Only marked submissions can be moderated."
        );
    }

    submission.transition(SubmissionState::MODERATED);
    return true;
}

bool SubmissionService::releaseSubmission(
    User& hod,
    Submission& submission
) {
    if (!hod.isActive()) {
        throw std::runtime_error(
            "Inactive users cannot release marks."
        );
    }

    if (!hod.haspermission(Permission::RELEASE_MARKS)) {
        throw std::runtime_error(
            "You do not have permission to release marks."
        );
    }

    if (submission.getState() != SubmissionState::MODERATED) {
        throw std::runtime_error(
            "Only moderated submissions can be released."
        );
    }

    submission.release();
    return submission.getState() == SubmissionState::RELEASED;
}
