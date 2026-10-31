#ifndef STUDENT_H
#define STUDENT_H

#include "user.h"
#include "enrollment.h"
#include <vector>

using std::vector;

class Student : public User {
private:
    vector<Enrollment> enrollments;

public:
    Student(UserId id, const string& username, const string& passwordhash);

    string getRole() const override {
        return "Student";
    }

    virtual bool haspermission(Permission action) const override;

    void addEnrollment(const Enrollment& enrollment);
};

#endif
