/**
 * @file
 * @brief Definition and implementation of logging functions and macros
 */
#ifndef IMMORTAL_STACK_LOGGER_H
#define IMMORTAL_STACK_LOGGER_H

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include "environment.h"

static FILE* _logFile = nullptr;

constexpr const char* defaultLogFileName = "log.txt";

/**
 * Closes the current log file.
 */
inline void logClose() {
    if (_logFile != nullptr) {
        fclose(_logFile);
        _logFile = nullptr;
    }
}

/**
 * Opens a file for logging.
 * @param[in] logFilePath path to the file to log info in
 * @param[in] modes       modes to open file in
 */
inline void logOpen(const char* logFilePath = defaultLogFileName, const char* modes = "a+") {
    assert(logFilePath != nullptr);
    assert(modes != nullptr);

    if (_logFile != nullptr) logClose();
    _logFile = fopen(logFilePath, modes);
}

/**
 * Prints formatted string (like printf or fprintf) in the log file.
 */
inline void logPrintf(const char* format, ...) {
    assert(_logFile != nullptr);
    assert(format != nullptr);

    va_list args;
    va_start(args, format);
    vfprintf(_logFile, format, args);
    va_end(args);
}

//----------------------------------------------------------------------------------------------------------------------

/**
 * Logs int value into log file.
 */
inline void logValue(int value) {
    logPrintf("%d", value);
}

/**
 * Logs unsigned int value into log file.
 */
inline void logValue(unsigned int value) {
    logPrintf("%u", value);
}

/**
 * Logs long int value into log file.
 */
inline void logValue(long int value) {
    logPrintf("%ld", value);
}

/**
 * Logs unsigned long int value into log file.
 */
inline void logValue(unsigned long int value) {
    logPrintf("%zu", value);
}

/**
 * Logs long long value into log file.
 */
inline void logValue(long long value) {
    logPrintf("%lld", value);
}

/**
 * Logs unsigned long long value into log file.
 */
inline void logValue(unsigned long long value) {
    logPrintf("%llu", value);
}

/**
 * Logs short value into log file.
 */
inline void logValue(short value) {
    logPrintf("%hd", value);
}

/**
 * Logs unsigned short value into log file.
 */
inline void logValue(unsigned short value) {
    logPrintf("%hu", value);
}

/**
 * Logs float value into log file.
 */
inline void logValue(float value) {
    logPrintf("%lg", value);
}

/**
 * Logs double value into log file.
 */
inline void logValue(double value) {
    logPrintf("%lg", value);
}

/**
 * Logs long double value into log file.
 */
inline void logValue(long double value) {
    logPrintf("%Lg", value);
}

/**
 * Logs char value into log file.
 */
inline void logValue(char value) {
    logPrintf("%c", value);
}

/**
 * Logs unsigned char value into log file.
 */
inline void logValue(unsigned char value) {
    logPrintf("%hhu", value);
}

/**
 * Logs const char* value into log file.
 */
inline void logValue(const char* value) {
    logPrintf("%s", value);
}

/**
 * Logs bool value into log file.
 */
inline void logValue(bool value) {
    logPrintf("%s", value ? "true" : "false");
}

//----------------------------------------------------------------------------------------------------------------------

/**
 * Logs value of any supported type into log file.
 * Logged value is preceded by the given indent.
 */
#define LOG_VALUE_INDENTED(value, indent) do {                                                                         \
    logPrintf(indent "%s = ", #value);                                                                                 \
    logValue(value);                                                                                                   \
    logPrintf("\n");                                                                                                   \
} while (0)

/**
 * Logs value of any supported type into log file.
 */
#define LOG_VALUE(value) LOG_VALUE_INDENTED(value, "")

/**
 * Logs array of any supported type into log file.
 * Logged array is preceded by the given indent.
 */
#define LOG_ARRAY_INDENTED(array, length, indent) do {                                                                 \
    logPrintf(indent "%s [" PTR_FORMAT "]", #array, (uintptr_t)array);                                                 \
    if (array == nullptr) {                                                                                            \
        logPrintf("\n");                                                                                               \
        break;                                                                                                         \
    }                                                                                                                  \
    logPrintf(" = {\n");                                                                                               \
    for (size_t i = 0; i < (length); ++i) {                                                                            \
        logPrintf(indent "\t[%zu] = ", i);                                                                             \
        logValue(array[i]);                                                                                            \
        logPrintf("\n");                                                                                               \
    }                                                                                                                  \
    logPrintf(indent "}\n");                                                                                           \
} while (0)

/**
 * Logs array of any supported type into log file.
 */
#define LOG_ARRAY(array, length) LOG_ARRAY_INDENTED(array, length, "")

#endif // IMMORTAL_STACK_LOGGER_H
