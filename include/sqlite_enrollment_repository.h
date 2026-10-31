
#ifndef SQLITE_ENROLLMENT_REPOSITORY_H
#define SQLITE_ENROLLMENT_REPOSITORY_H

#include "sqlite_repository.h"
#include "enrollment.h"
#include <vector>

class SQLiteEnrollmentRepository : public SQLiteRepository {
public:
    explicit SQLiteEnrollmentRepository(SQLiteConnection& connection);

    void initializeTable();

    bool enrollStudent(int studentId, int courseId);

    std::vector<Enrollment> getAllEnrollments();
    bool isStudentEnrolled(int studentId, int courseId); 
};

#endif
