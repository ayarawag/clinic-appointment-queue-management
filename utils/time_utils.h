#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <string>

namespace TimeUtils {
    // Returns "YYYY-MM-DD HH:MM:SS"
    std::string getCurrentDateTime();

    // Basic format validation: "YYYY-MM-DD HH:MM:SS"
    bool isValidDateTime(const std::string& dt);

    // returns current epoch seconds
    long long nowEpochSeconds();
}

#endif // TIME_UTILS_H