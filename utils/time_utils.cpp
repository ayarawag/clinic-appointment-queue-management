#include "time_utils.h"
#include <ctime>
#include <regex>

namespace TimeUtils {

std::string getCurrentDateTime() {
    time_t now = time(0);
    tm* local = localtime(&now);

    char buffer[30];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local);
    return buffer;
}

bool isValidDateTime(const std::string& dt) {
    std::regex pattern(
        R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}$)"
    );
    return std::regex_match(dt, pattern);
}

}