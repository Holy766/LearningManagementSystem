
#ifndef SUBMISSION_SERVICE_H
#define SUBMISSION_SERVICE_H

#include "submission.h"
#include "assignment.h"
#include "user.h"
#include "mark_audit_manager.h"

class SubmissionService {
public:
    bool submitWork(const User& student, Submission& submission,const Assignment& assignment,
        const string& fileReference, const Datetime& submittedAt
    );

    bool resubmitWork(const User& student,Submission& submission,const Assignment& assignment,
        const string& fileReference,const Datetime& submittedAt
    );
    
    bool moderateSubmission(const User& hod, Submission& submission
    );

    bool releaseSubmission(const User& hod,Submission& submission
    );

};

#endif
