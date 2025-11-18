#pragma once
#include <string>
bool isValidEmail(const std::string &email);
bool isValidPassword(const std::string &password);
std::string hashPassword(const std::string &password);
