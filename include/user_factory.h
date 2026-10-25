
#ifndef USER_FACTORY_H
#define USER_FACTORY_H

#include "interface_user_factory.h"

class UserFactory : public InterfaceUserFactory {
public:
    User* createUser(
        string role,
        UserId id,
        string username,
        string passwordhash
    ) override;
};

#endif
