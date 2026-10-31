
#ifndef SQLITE_SUBMISSION_REPOSITORY_H
#define SQLITE_SUBMISSION_REPOSITORY_H

#include "sqlite_repository.h"
#include "submission.h"
#include <optional>
#include <vector>

class SQLiteSubmissionRepository : public SQLiteRepository {
public:
    explicit SQLiteSubmissionRepository(SQLiteConnection& connection);

    void initializeTable();

    bool save(const Submission& submission);

    std::optional<Submission> findById(SubmissionId id);

    bool update(const Submission& submission);

    bool remove(SubmissionId id);
    std::vector<Submission> findByAssignmentId(AssignmentId assignmentId);
    SubmissionId getNextSubmissionId();
};

#endif
