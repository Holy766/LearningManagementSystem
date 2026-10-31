
#ifndef SQLITE_COURSE_REPOSITORY_H
#define SQLITE_COURSE_REPOSITORY_H

#include "sqlite_repository.h"
#include "course.h"
#include <vector>

class SQLiteCourseRepository : public SQLiteRepository {
public:
    explicit SQLiteCourseRepository(SQLiteConnection& connection);

    void initializeTable();

    bool addCourse(const Course& course);

    std::vector<Course> getAllCourses();
};

#endif

