#include <blazar/common/logger.h>

Logger g_logger = {.min_level = LOG_LEVEL_DEBUG,
                   .file = NULL,
                   .include_timestamp = true,
                   .include_file_line = true};

int main(void)
{
    LOG_DEBUG("test");
    LOG_INFO("test");
    return 0;
}
