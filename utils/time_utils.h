#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <string>

namespace TimeUtils {
    std::string getCurrentDateTime();
    bool isValidDateTime(const std::string& dt);
}

#endif