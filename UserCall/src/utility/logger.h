#pragma once
#include <Windows.h>
#include <cstdio>
#include <cstdarg>

enum class LogLevel {
    LOG_ERROR = 0,
    LOG_SUCCESS,
    LOG_INFO,
};

class Logger {
public:
    static void Log(LogLevel level, const char* format, ...);
};