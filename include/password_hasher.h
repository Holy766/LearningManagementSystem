#ifndef PASSWORD_HASHER_H
#define PASSWORD_HASHER_H

#include <string>

class password_hasher {
public:
    static std::string hashPassword(const std::string& password);

    static bool verifyPassword(
        const std::string& password,
        const std::string& storedHash
    );
};

#endif