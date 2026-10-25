
#ifndef INTERFACE_USER_FACTORY_H
#define INTERFACE_USER_FACTORY_H

#include <string>
#include "user.h"

using std::string;

class InterfaceUserFactory {
public:
    virtual User* createUser(
        string role,
        UserId id,
        string username,
        string passwordhash
    ) = 0;

    virtual ~InterfaceUserFactory() = default;
};

#endif
