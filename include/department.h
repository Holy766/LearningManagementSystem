
#ifndef DEPARTMENT_H
#define DEPARTMENT_H

#include <string>
#include "user.h"

using std::string;
using DepartmentId = int;

class Department {
private:
    DepartmentId id;
    string name;
    UserId hodId;  // Identifies the Head of Department

public:
    Department(DepartmentId id, const string& name, UserId hodId);

    DepartmentId getId() const;
    const string& getName() const;
    UserId getHodId() const;

    void setName(const string& name);
    void setHodId(UserId hodId);
};

#endif
