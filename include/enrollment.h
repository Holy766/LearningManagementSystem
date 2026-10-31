
#ifndef ENROLLMENT_H
#define ENROLLMENT_H

#include "user.h"
#include "datetime.h"
#include "ids.h"

using EnrollmentId = int;

class Enrollment {
private:
    EnrollmentId id;
    UserId studentId;
    CourseId courseId;
    Datetime enrolledAt;

public:
    Enrollment(EnrollmentId id, UserId studentId,
               CourseId courseId, const Datetime& enrolledAt);

    EnrollmentId getId() const;
    UserId getStudentId() const;
    CourseId getCourseId() const;
    Datetime getEnrolledAt() const;
};

#endif
