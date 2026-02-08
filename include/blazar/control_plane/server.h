#ifndef BLAZAR_CONTROL_PLANE_SERVER_H
#define BLAZAR_CONTROL_PLANE_SERVER_H

#include <blazar/common/ev_loop.h>
#include <stdint.h>

typedef struct cplane_server cplane_server_t;

struct cplane_server {
    int listen_fd;
    ev_io_t accept_watcher;
    ev_loop_t *loop;
};

cplane_server_t *cplane_server_new(ev_loop_t *loop, const char *ip,
                                   uint16_t port);
void cplane_server_destroy(cplane_server_t *server);

#endif // BLAZAR_CONTROL_PLANE_SERVER_H
