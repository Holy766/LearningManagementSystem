
#ifndef SUBMISSION_SERVICE_H
#define SUBMISSION_SERVICE_H

#include "submission.h"
#include "assignment.h"
#include "user.h"
#include "mark_audit_manager.h"

class SubmissionService {
public:
    static bool submitWork(const User& student, Submission& submission,const Assignment& assignment,
        const string& fileReference, const Datetime& submittedAt
    );

    static bool resubmitWork(const User& student,Submission& submission,const Assignment& assignment,
        const string& fileReference,const Datetime& submittedAt
    );
    
    static bool moderateSubmission(const User& hod, Submission& submission
    );

    static bool releaseSubmission(const User& hod,Submission& submission
    );

};

#endif
