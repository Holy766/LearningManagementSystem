
#ifndef USER_H
#define USER_H

#include <string>
#include "password_hasher.h"

using std::string;

using UserId = int;


enum class Permission {
    VIEW_COURSES,
    VIEW_ASSIGNMENTS,
    SUBMIT_WORK,
    CREATE_ASSIGNMENT,
    MARK_SUBMISSION,
    MODERATE_MARKS,
    RELEASE_MARKS,
    MANAGE_USERS,
    MANAGE_DEPARTMENTS,
    MANAGE_COURSES,
    ASSIGN_LECTURERS,
    ENROLL_STUDENTS,
    GRANT_EXTENSIONS
};

class User {
private:
    string username;
    // string salt;
    bool active;
    UserId id;
    string passwordhash;

public:
    User(UserId id, const string& username, const string& password);

    UserId getId() const;
    string getusername() const;
    virtual bool haspermission(Permission action) const;
    bool isActive() const;
    virtual ~User()=default;
    bool verifyPassword(const string& password) const;
    virtual string getRole() const = 0;
};

#endif
