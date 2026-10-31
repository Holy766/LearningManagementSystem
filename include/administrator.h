#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H

#include "user.h"

class Administrator : public User {
public:
    Administrator(UserId id, const string& username,
                  const string& passwordhash);

    virtual bool haspermission(Permission action) const override;
    string getRole() const override {
        return "Administrator";
    }
};

#endif
