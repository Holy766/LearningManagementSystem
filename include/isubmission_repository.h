
#ifndef ISUBMISSION_REPOSITORY_H
#define ISUBMISSION_REPOSITORY_H

#include "submission.h"
#include "irepository.h"
#include <optional>

class ISubmissionRepository : public IRepository {
public:
    virtual ~ISubmissionRepository() = default;

    virtual bool save(const Submission& submission) = 0;
    virtual std::optional<Submission> findById(SubmissionId id) = 0;
    virtual bool update(const Submission& submission) = 0;
    virtual bool remove(SubmissionId id) = 0;
    virtual std::vector<Submission> findByAssignmentId(AssignmentId assignmentId) = 0;
};

#endif
