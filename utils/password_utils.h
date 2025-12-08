#ifndef PASSWORD_UTILS_H
#define PASSWORD_UTILS_H

#include <string>

namespace PasswordUtils {
    std::string hashPassword(const std::string& pass);
    bool verifyPassword(const std::string& pass, const std::string& hashed);
}

#endif