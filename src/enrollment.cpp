
#include "enrollment.h"

Enrollment::Enrollment(EnrollmentId id, UserId studentId,
                       CourseId courseId, const Datetime& enrolledAt): id(id),
      studentId(studentId),
      courseId(courseId),
      enrolledAt(enrolledAt) {
}

EnrollmentId Enrollment::getId() const {
    return id;
}

UserId Enrollment::getStudentId() const {
    return studentId;
}

CourseId Enrollment::getCourseId() const {
    return courseId;
}

Datetime Enrollment::getEnrolledAt() const {
    return enrolledAt;
}
