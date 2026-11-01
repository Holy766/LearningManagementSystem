
#include "department.h"

Department::Department(DepartmentId id,const string& name, UserId hodId): id(id), name(name), hodId(hodId) {
}

// Getters

DepartmentId Department::getId() const {
    return id;
}

string Department::getName() const {
    return name;
}

const string& Department::getName() const {
    return name;
}

UserId Department::getHodId() const {
    return hodId;
}

// Setters

void Department::setName(const string& name) {
    this->name = name;
}

void Department::setHodId(UserId hodId) {
    this->hodId = hodId;
}
