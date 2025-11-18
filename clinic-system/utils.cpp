#include "utils.h"
#include <string>
#include <regex>

bool isValidEmail(const std::string &email) {
    const std::regex pattern(R"((\w+)(\.\w+)*@(\w+)(\.\w+)+)");
    return std::regex_match(email, pattern);
}

bool isValidPassword(const std::string &password) {
    return password.length() >= 8;
}

std::string hashPassword(const std::string &password) {
    std::string hashed = "";
    for (char c : password) hashed += std::to_string((int)c + 7);
    return hashed;
}
