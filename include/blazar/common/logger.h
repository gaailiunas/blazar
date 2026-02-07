#ifndef BLAZAR_COMMON_LOGGER_H
#define BLAZAR_COMMON_LOGGER_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

typedef enum {
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG = 1,
    LOG_LEVEL_INFO = 2,
    LOG_LEVEL_WARN = 3,
    LOG_LEVEL_ERROR = 4,
    LOG_LEVEL_FATAL = 5
} LogLevel;

typedef struct {
    LogLevel min_level;
    FILE *file;
    bool include_timestamp : 1;
    bool include_file_line : 1;
} Logger;

extern Logger g_logger;

void logger_init(LogLevel min_level, const char *log_file);
void logger_cleanup(void);
void logger_set_level(LogLevel level);

void logger_log(LogLevel level, const char *file, int line, const char *fmt,
                ...);

#ifdef NDEBUG
#define LOG_TRACE(...) ((void)0)
#define LOG_DEBUG(...) ((void)0)
#else
#define LOG_TRACE(...)                                                         \
    logger_log(LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...)                                                         \
    logger_log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#endif

#define LOG_INFO(...)                                                          \
    logger_log(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)                                                          \
    logger_log(LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)                                                         \
    logger_log(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...)                                                         \
    logger_log(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#endif // BLAZAR_COMMON_LOGGER_H
