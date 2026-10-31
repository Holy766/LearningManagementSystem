#ifndef HEAD_OF_DEPARTMENT_H
#define HEAD_OF_DEPARTMENT_H

#include "user.h"

class HoD : public User {
public:
    HoD(UserId id, const string& username,const string& passwordhash);

    virtual bool haspermission(Permission action) const override;
    string getRole() const override {
        return "Head of Department";
    }
};

#endif
