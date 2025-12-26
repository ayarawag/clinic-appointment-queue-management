#include "password_utils.h"
#include <string>
#include <sstream>

static std::string simpleHash(const std::string& input) {
    unsigned int h = 0;
    for (char c : input)
        h = (h * 131) + c;
    return std::to_string(h);
}

namespace PasswordUtils {

std::string hashPassword(const std::string& pass) {
    return simpleHash(pass);
}

bool verifyPassword(const std::string& pass, const std::string& hashed) {
    return simpleHash(pass) == hashed;
}

} // namespace PasswordUtils