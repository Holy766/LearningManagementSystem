
#ifndef IASSIGNMENT_REPOSITORY_H
#define IASSIGNMENT_REPOSITORY_H

#include "assignment.h"
#include <optional>
#include <vector>

class IAssignmentRepository {
public:
    virtual ~IAssignmentRepository() = default;

    virtual bool save(const Assignment& assignment) = 0;
    virtual std::optional<Assignment> findById(AssignmentId id) = 0;
    virtual std::vector<Assignment> findByCourseId(CourseId courseId) = 0;
    virtual bool update(const Assignment& assignment) = 0;
    virtual bool remove(AssignmentId id) = 0;
};

#endif

