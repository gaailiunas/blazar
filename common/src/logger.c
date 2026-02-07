#include <blazar/common/logger.h>
#include <string.h>
#include <time.h>

#define COLOR_RESET "\x1b[0m"
#define COLOR_TRACE "\x1b[94m" // Light blue
#define COLOR_DEBUG "\x1b[36m" // Cyan
#define COLOR_INFO "\x1b[32m"  // Green
#define COLOR_WARN "\x1b[33m"  // Yellow
#define COLOR_ERROR "\x1b[31m" // Red
#define COLOR_FATAL "\x1b[35m" // Magenta

static const char *level_names[] = {"TRACE", "DEBUG", "INFO",
                                    "WARN",  "ERROR", "FATAL"};

static const char *level_colors[] = {COLOR_TRACE, COLOR_DEBUG, COLOR_INFO,
                                     COLOR_WARN,  COLOR_ERROR, COLOR_FATAL};

void logger_init(LogLevel min_level, const char *log_file)
{
    g_logger.min_level = min_level;

    if (log_file) {
        g_logger.file = fopen(log_file, "a");
        if (!g_logger.file) {
            fprintf(stderr, "Failed to open log file: %s\n", log_file);
        }
    }
}

void logger_cleanup(void)
{
    if (g_logger.file) {
        fclose(g_logger.file);
        g_logger.file = NULL;
    }
}

void logger_set_level(LogLevel level) { g_logger.min_level = level; }

void logger_log(LogLevel level, const char *file, int line, const char *fmt,
                ...)
{
    if (level < g_logger.min_level) {
        return;
    }

    time_t now;
    time(&now);
    struct tm *tm_info = localtime(&now);
    char time_buffer[64];

    if (g_logger.include_timestamp) {
        strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S",
                 tm_info);
    }

    const char *filename = strrchr(file, '/');
    if (!filename) {
        filename = strrchr(file, '\\');
    }
    filename = filename ? filename + 1 : file;

    fprintf(stderr, "%s", level_colors[level]);

    if (g_logger.include_timestamp) {
        fprintf(stderr, "[%s] ", time_buffer);
    }

    fprintf(stderr, "[%s]", level_names[level]);

    if (g_logger.include_file_line) {
        fprintf(stderr, " [%s:%d]", filename, line);
    }

    fprintf(stderr, " ");

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "%s\n", COLOR_RESET);

    if (g_logger.file) {
        if (g_logger.include_timestamp) {
            fprintf(g_logger.file, "[%s] ", time_buffer);
        }

        fprintf(g_logger.file, "[%s]", level_names[level]);

        if (g_logger.include_file_line) {
            fprintf(g_logger.file, " [%s:%d]", filename, line);
        }

        fprintf(g_logger.file, " ");

        va_start(args, fmt);
        vfprintf(g_logger.file, fmt, args);
        va_end(args);

        fprintf(g_logger.file, "\n");
        fflush(g_logger.file);
    }
}
