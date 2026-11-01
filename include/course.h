
#ifndef COURSE_H
#define COURSE_H

#include <string>
#include <vector>
#include "ids.h"
#include "assignment.h"
#include "enrollment.h"

using std::string;
using std::vector;

class Course {
private:
    CourseId id;
    DepartmentId departmentId;
    string code;
    string title;
    vector<Assignment> assignments;
    vector<Enrollment> enrollments;

public:
    Course(CourseId id, DepartmentId departmentId,
          const string& code, const string& title);

    CourseId getId() const;
    DepartmentId getDepartmentId() const;
    const string& getCode() const;
    const string& getTitle() const;

    void setCode(const string& code);
    void setTitle(const string& title);

    void addAssignment(const Assignment& assignment);
    void displayAssignments() const;
};

#endif
