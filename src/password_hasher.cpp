
#include "password_hasher.h"
#include <argon2.h>
#include <random>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <iomanip>

std::string password_hasher::hashPassword(const std::string& password) {
    if (password.empty()) {
        throw std::invalid_argument("Password cannot be empty.");
    }

    const uint32_t t_cost = 3;
    const uint32_t m_cost = 65536;
    const uint32_t parallelism = 1;
    const size_t saltLength = 16;
    const size_t hashLength = 32;

    std::vector<uint8_t> salt(saltLength);
    std::random_device rd;

    for (size_t i = 0; i < saltLength; ++i) {
        salt[i] = static_cast<uint8_t>(rd());
    }

    std::vector<uint8_t> hash(hashLength);

    int result = argon2id_hash_raw(
        t_cost,
        m_cost,
        parallelism,
        password.data(),
        password.size(),
        salt.data(),
        salt.size(),
        hash.data(),
        hash.size()
    );

    if (result != ARGON2_OK) {
        throw std::runtime_error("Password hashing failed.");
    }

    std::ostringstream storedHash;
    storedHash << std::hex << std::setfill('0');

    for (uint8_t byte : salt) {
        storedHash << std::setw(2) << static_cast<unsigned int>(byte);
    }

    storedHash << ':';

    for (uint8_t byte : hash) {
        storedHash << std::setw(2) << static_cast<unsigned int>(byte);
    }

    return storedHash.str();
}

bool password_hasher::verifyPassword(
    const std::string& password,
    const std::string& storedHash
) {
    if (password.empty()) {
        return false;
    }

    size_t separator = storedHash.find(':');

    if (separator == std::string::npos || separator != 32) {
        return false;
    }

    std::vector<uint8_t> salt(16);
    std::vector<uint8_t> expectedHash(32);

    auto hexToBytes = [](const std::string& hex,
                         std::vector<uint8_t>& bytes) -> bool {
        if (hex.size() != bytes.size() * 2) {
            return false;
        }

        for (size_t i = 0; i < bytes.size(); ++i) {
            try {
                bytes[i] = static_cast<uint8_t>(
                    std::stoul(hex.substr(i * 2, 2), nullptr, 16)
                );
            } catch (...) {
                return false;
            }
        }

        return true;
    };

    if (!hexToBytes(storedHash.substr(0, separator), salt) ||
        !hexToBytes(storedHash.substr(separator + 1), expectedHash)) {
        return false;
    }

    std::vector<uint8_t> actualHash(32);

    int result = argon2id_hash_raw(
        3,
        65536,
        1,
        password.data(),
        password.size(),
        salt.data(),
        salt.size(),
        actualHash.data(),
        actualHash.size()
    );

    if (result != ARGON2_OK) {
        return false;
    }

    uint8_t difference = 0;

    for (size_t i = 0; i < actualHash.size(); ++i) {
        difference |= actualHash[i] ^ expectedHash[i];
    }

    return difference == 0;
}
