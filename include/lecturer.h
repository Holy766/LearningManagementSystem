#ifndef LECTURER_H
#define LECTURER_H

#include "user.h"

class Lecturer : public User {
public:
    Lecturer(UserId id, const string& username,const string& passwordhash);

    virtual bool haspermission(Permission action) const override;
    string getRole() const override {
    return "Lecturer";
}
};

#endif