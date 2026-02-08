#include <blazar/common/ev_loop.h>
#include <blazar/common/logger.h>
#include <blazar/control_plane/server.h>

Logger g_logger = {.min_level = LOG_LEVEL_DEBUG,
                   .file = NULL,
                   .include_timestamp = true,
                   .include_file_line = true};

int main(void)
{
    ev_loop_t *loop = ev_loop_new();
    if (!loop) {
        LOG_ERROR("Failed to create a new event loop");
        return 1;
    }

    cplane_server_t *server = cplane_server_new(loop, "127.0.0.1", 8000);
    if (!server) {
        LOG_ERROR("Failed to create a new control plane server");
        ev_loop_destroy(loop);
        return 1;
    }

    LOG_INFO("The control plane server is now running");

    ev_loop_run(loop);
    ev_loop_destroy(loop);
    return 0;
}
