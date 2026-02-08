#include <arpa/inet.h>
#include <blazar/common/ev_loop.h>
#include <blazar/common/logger.h>
#include <blazar/common/sock.h>
#include <blazar/control_plane/client.h>
#include <blazar/control_plane/server.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static void cplane_server__accept_cb(ev_loop_t *loop, ev_io_t *watcher,
                                     int revents)
{
    cplane_server_t *server = (cplane_server_t *)watcher->data;
    if (revents & EV_READ) {
        int cfd = accept(server->listen_fd, NULL, NULL);
        if (cfd < 0) {
            LOG_WARN("Failed to accept control plane client");
            return;
        }

        LOG_DEBUG("New connection accepted, fd:%d", cfd);

        if (set_nonblocking(cfd) < 0) {
            LOG_ERROR("Failed to set control plane client as nonblocking");
            close(cfd);
            return;
        }

        cplane_client_t *client = cplane_client_new(cfd);
        if (ev_io_start(loop, &client->watcher) < 0) {
            LOG_ERROR("Failed to start control plane client watcher");
            close(cfd);
            free(client);
            return;
        }
    }
}

cplane_server_t *cplane_server_new(ev_loop_t *loop, const char *ip,
                                   uint16_t port)
{
    struct sockaddr_in sin;

    cplane_server_t *server = (cplane_server_t *)malloc(sizeof(*server));
    if (!server) {
        return NULL;
    }

    server->loop = loop;
    server->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->listen_fd < 0) {
        LOG_ERROR("Failed to create a new control plane server socket");
        free(server);
        return NULL;
    }

    int opt = 1;
    setsockopt(server->listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (set_nonblocking(server->listen_fd) < 0) {
        LOG_ERROR("Failed to set control plane socket to nonblocking");
        close(server->listen_fd);
        free(server);
        return NULL;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &sin.sin_addr) != 1) {
        LOG_ERROR("Invalid numeric ipv4");
        close(server->listen_fd);
        free(server);
        return NULL;
    }

    if (bind(server->listen_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        LOG_ERROR("Failed to bind control plane server");
        close(server->listen_fd);
        free(server);
        return NULL;
    }

    if (listen(server->listen_fd, 128) < 0) {
        LOG_ERROR("Failed to listen control plane server");
        close(server->listen_fd);
        free(server);
        return NULL;
    }

    ev_io_init(&server->accept_watcher, cplane_server__accept_cb,
               server->listen_fd, EV_READ);
    server->accept_watcher.data = server;

    if (ev_io_start(loop, &server->accept_watcher) < 0) {
        LOG_ERROR("Failed to start accept watcher of the control plane server");
        close(server->listen_fd);
        free(server);
        return NULL;
    }

    return server;
}

void cplane_server_destroy(cplane_server_t *server)
{
    if (!server) {
        return;
    }

    // TODO: gracefully exit all sockets from io_watchers llist

    ev_io_stop(server->loop, &server->accept_watcher);
    close(server->listen_fd);
    free(server);
}
