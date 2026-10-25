
#include "course.h"

Course::Course(CourseId id, DepartmentId departmentId,string code, string title): id(id), departmentId(departmentId),
      code(code), title(title) {
}

// Getters

CourseId Course::getId() const {
    return id;
}

DepartmentId Course::getDepartmentId() const {
    return departmentId;
}

string Course::getCode() const {
    return code;
}

string Course::getTitle() const {
    return title;
}

// Setters

void Course::setCode(string code) {
    this->code = code;
}

void Course::setTitle(string title) {
    this->title = title;
}

void Course::addAssignment(const Assignment& assignment) {
    assignments.push_back(assignment);
}

void Course::displayAssignments() const {
    for (const Assignment& assignment : assignments) {
        assignment.display();
    }
}