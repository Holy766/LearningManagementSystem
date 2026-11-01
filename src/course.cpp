
#include "course.h"

Course::Course(CourseId id, DepartmentId departmentId,const string& code, const string& title): id(id), departmentId(departmentId),code(code), title(title) {
}

// Getters

CourseId Course::getId() const {
    return id;
}


DepartmentId Course::getDepartmentId() const {
    return departmentId;
}

const string& Course::getCode() const {
    return code;
}

const string& Course::getTitle() const {
    return title;
}

// Setters

void Course::setCode(const string& code) {
    this->code = code;
}

void Course::setTitle(const string& title) {
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