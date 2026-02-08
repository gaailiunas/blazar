#ifndef BLAZAR_CONTROL_PLANE_CLIENT_H
#define BLAZAR_CONTROL_PLANE_CLIENT_H

#include <blazar/common/ev_loop.h>

typedef struct cplane_client cplane_client_t;

struct cplane_client {
    int fd;
    ev_io_t watcher;
};

cplane_client_t *cplane_client_new(int fd);
void cplane_client_destroy(cplane_client_t *client, ev_loop_t *loop);

#endif // BLAZAR_CONTROL_PLANE_CLIENT_H
