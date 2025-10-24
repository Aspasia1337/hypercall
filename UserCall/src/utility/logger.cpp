#include "logger.h"

void Logger::Log(LogLevel level, const char* format, ...) {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    switch (level) {
    case LogLevel::LOG_ERROR:
        SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
        break;
    case LogLevel::LOG_SUCCESS:
        SetConsoleTextAttribute(consoleHandle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        break;
    case LogLevel::LOG_INFO:
    default:
        SetConsoleTextAttribute(consoleHandle, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        break;
    }

    const char* tag =
        (level == LogLevel::LOG_ERROR) ? "[-]" :
        (level == LogLevel::LOG_SUCCESS) ? "[+]" :
        "[INFO]";

    printf("%s ", tag);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");

    SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}