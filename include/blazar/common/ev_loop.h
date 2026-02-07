#ifndef BLAZAR_COMMON_EV_LOOP_H
#define BLAZAR_COMMON_EV_LOOP_H

#include <stdbool.h>

typedef struct ev_loop ev_loop_t;
typedef struct ev_io ev_io_t;

typedef enum {
    EV_READ = 0x01,
    EV_WRITE = 0x02,
    EV_ERROR = 0x04,
} ev_event_t;

typedef void (*ev_io_cb)(ev_loop_t *loop, ev_io_t *watcher, int revents);

struct ev_io {
    int fd;
    int events;
    ev_io_cb callback;
    void *data;

    struct ev_io *next;
    struct ev_io *prev;
};

struct ev_loop {
    bool running;
    int epoll_fd;
    ev_io_t *io_watchers;
    void *user_data;
};

/**
 * Create a new event loop
 * Returns NULL on failure
 */
ev_loop_t *ev_loop_new(void);

/**
 * Destroy event loop and free resources
 */
void ev_loop_destroy(ev_loop_t *loop);

/**
 * Run the event loop
 * Returns 0 on success, -1 on error
 */
int ev_loop_run(ev_loop_t *loop);

/**
 * Stop the event loop
 */
void ev_loop_stop(ev_loop_t *loop);

/**
 * Initialize an I/O watcher
 */
void ev_io_init(ev_io_t *watcher, ev_io_cb callback, int fd, int events);

/**
 * Start watching for I/O events
 * Returns 0 on success, -1 on error
 */
int ev_io_start(ev_loop_t *loop, ev_io_t *watcher);

/**
 * Stop watching for I/O events
 */
void ev_io_stop(ev_loop_t *loop, ev_io_t *watcher);

/**
 * Modify events being watched
 */
int ev_io_modify(ev_loop_t *loop, ev_io_t *watcher, int events);

#endif // BLAZAR_COMMON_EV_LOOP_H
