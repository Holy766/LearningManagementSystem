
#include "user_factory.h"
#include "student.h"
#include "lecturer.h"
#include "head_of_department.h"
#include "administrator.h"

User* UserFactory::createUser(
    string role,
    UserId id,
    string username,
    string passwordhash
) {
    if (role == "Student") {
        return new Student(id, username, passwordhash);
    }
    else if (role == "Lecturer") {
        return new Lecturer(id, username, passwordhash);
    }
    else if (role == "HoD") {
        return new HoD(id, username, passwordhash);
    }
    else if (role == "Administrator") {
        return new Administrator(id, username, passwordhash);
    }

    return nullptr;
}
