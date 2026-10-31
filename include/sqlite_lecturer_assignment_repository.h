
#ifndef SQLITE_LECTURER_ASSIGNMENT_REPOSITORY_H
#define SQLITE_LECTURER_ASSIGNMENT_REPOSITORY_H

#include "sqlite_repository.h"
#include <vector>

class SQLiteLecturerAssignmentRepository : public SQLiteRepository {
public:
    explicit SQLiteLecturerAssignmentRepository(SQLiteConnection& connection);
    std::vector<int> getCoursesForLecturer(int lecturerId);
    void initializeTable();

    bool assignLecturer(int courseId, int lecturerId);

    int getLecturerForCourse(int courseId);

};

#endif
