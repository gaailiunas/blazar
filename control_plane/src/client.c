#include <blazar/common/ev_loop.h>
#include <blazar/common/logger.h>
#include <blazar/control_plane/client.h>
#include <stdlib.h>
#include <unistd.h>

static void cplane_client__io_cb(ev_loop_t *loop, ev_io_t *watcher, int revents)
{
    cplane_client_t *client = (cplane_client_t *)watcher->data;

    if (revents & EV_ERROR) {
        LOG_WARN("cplane client error, closing connection");
        cplane_client_destroy(client, loop);
        return;
    }
    if (revents & EV_READ) {
        // TODO: arena allocators for buffering
        char buf[1024];
        ssize_t n = read(client->fd, buf, sizeof(buf));
        if (n <= 0) {
            if (n == 0) {
                LOG_DEBUG("cplane client disconnected");
            }
            else {
                LOG_WARN("cplane client error, closing connection");
            }
            cplane_client_destroy(client, loop);
            return;
        }
        LOG_DEBUG("Received %d bytes from fd:%d", n, client->fd);
        cplane_client_destroy(client, loop); // temporary
    }
}

cplane_client_t *cplane_client_new(int fd)
{
    cplane_client_t *client = (cplane_client_t *)malloc(sizeof(*client));
    if (!client) {
        return NULL;
    }

    client->fd = fd;
    ev_io_init(&client->watcher, cplane_client__io_cb, fd, EV_READ);
    client->watcher.data = client;
    return client;
}

void cplane_client_destroy(cplane_client_t *client, ev_loop_t *loop)
{
    if (!client) {
        return;
    }
    if (client->fd >= 0) {
        close(client->fd);
    }
    ev_io_stop(loop, &client->watcher);
    free(client);
}
