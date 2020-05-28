#pragma once

#include <string>

struct Settings
{
    static std::string logLevel;
    static size_t threadAmount;
    static std::string output;
    static std::string input;
    static bool writeOnly;
};