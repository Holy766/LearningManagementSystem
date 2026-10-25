
#ifndef SUBMISSION_SERVICE_H
#define SUBMISSION_SERVICE_H

#include "submission.h"
#include "assignment.h"
#include "user.h"
#include "mark_audit_manager.h"

class SubmissionService {
public:
    bool submitWork( User& student, Submission& submission,Assignment& assignment,
        const string& fileReference, Datetime submittedAt
    );

    bool resubmitWork(User& student,Submission& submission,Assignment& assignment,
        const string& fileReference,Datetime submittedAt
    );
    
    bool moderateSubmission(User& hod, Submission& submission
    );

    bool releaseSubmission(User& hod,Submission& submission
    );

};

#endif
