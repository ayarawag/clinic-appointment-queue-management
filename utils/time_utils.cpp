#include "time_utils.h"
#include <ctime>
#include <regex>

namespace TimeUtils {

std::string getCurrentDateTime() {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return std::string(buf);
}

bool isValidDateTime(const std::string& dt) {
    static const std::regex re(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}$)");
    return std::regex_match(dt, re);
}

long long nowEpochSeconds() {
    return static_cast<long long>(std::time(nullptr));
}

} // namespace TimeUtils