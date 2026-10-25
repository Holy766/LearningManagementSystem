#ifndef SQLITE_ASSIGNMENT_REPOSITORY_H
#define SQLITE_ASSIGNMENT_REPOSITORY_H

#include "sqlite_repository.h"
#include "iassignment_repository.h"

class SQLiteAssignmentRepository : public SQLiteRepository,public IAssignmentRepository {
public:
    explicit SQLiteAssignmentRepository(SQLiteConnection& connection);

    void initializeTable();

    bool save(const Assignment& assignment) override;

    std::optional<Assignment> findById(AssignmentId id) override;

    std::vector<Assignment> findByCourseId(CourseId courseId) override;

    bool update(const Assignment& assignment) override;

    bool remove(AssignmentId id) override;
};

#endif
