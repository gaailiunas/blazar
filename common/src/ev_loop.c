#include <blazar/common/ev_loop.h>
#include <blazar/common/logger.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#define MAX_EVENTS 64
#define TIMEOUT 1

static void list_add(ev_io_t **head, ev_io_t *item)
{
    item->next = *head;
    item->prev = NULL;
    if (*head) {
        (*head)->prev = item;
    }
    *head = item;
}

static void list_remove(ev_io_t **head, ev_io_t *item)
{
    if (item->prev) {
        item->prev->next = item->next;
    }
    else {
        *head = item->next;
    }
    if (item->next) {
        item->next->prev = item->prev;
    }
    item->next = item->prev = NULL;
}

ev_loop_t *ev_loop_new(void)
{
    ev_loop_t *loop = (ev_loop_t *)malloc(sizeof(*loop));
    if (!loop) {
        return NULL;
    }

    loop->epoll_fd = epoll_create1(0);
    if (loop->epoll_fd < 0) {
        free(loop);
        return NULL;
    }

    loop->running = false;
    loop->io_watchers = NULL;
    loop->user_data = NULL;
    return loop;
}

void ev_loop_destroy(ev_loop_t *loop)
{
    if (!loop) {
        return;
    }

    if (loop->epoll_fd >= 0) {
        close(loop->epoll_fd);
    }

    // user owns io_watchers and is responsible for freeing the llist
    loop->io_watchers = NULL;
    free(loop);
}

int ev_loop_run(ev_loop_t *loop)
{
    int nfds;
    ev_io_t *watcher;
    int revents = 0;
    struct epoll_event events[MAX_EVENTS];

    if (!loop) {
        return -1;
    }

    loop->running = true;

    while (loop->running) {
        nfds = epoll_wait(loop->epoll_fd, events, MAX_EVENTS, TIMEOUT);
        if (nfds < 0) {
            if (errno == EINTR) {
                LOG_INFO("Event loop got interrupted");
                return 0;
            }
            return -1;
        }

        for (int i = 0; i < nfds; i++) {
            watcher = (ev_io_t *)events[i].data.ptr;
            if (events[i].events & EPOLLIN) {
                revents |= EV_READ;
            }
            if (events[i].events & EPOLLOUT) {
                revents |= EV_WRITE;
            }
            if (events[i].events & (EPOLLERR | EPOLLHUP)) {
                revents |= EV_ERROR;
            }
            if (watcher && watcher->callback) {
                watcher->callback(loop, watcher, revents);
            }
        }
    }

    return 0;
}

void ev_loop_stop(ev_loop_t *loop)
{
    if (loop) {
        loop->running = false;
    }
}

void ev_io_init(ev_io_t *watcher, ev_io_cb callback, int fd, int events)
{
    watcher->fd = fd;
    watcher->events = events;
    watcher->callback = callback;
    watcher->data = NULL;
    watcher->next = NULL;
    watcher->prev = NULL;
}

int ev_io_start(ev_loop_t *loop, ev_io_t *watcher)
{
    struct epoll_event ev;

    if (!loop || !watcher) {
        return -1;
    }

    ev.events = 0;
    if (watcher->events & EV_READ) {
        ev.events |= EPOLLIN;
    }
    if (watcher->events & EV_WRITE) {
        ev.events |= EPOLLOUT;
    }
    ev.data.ptr = watcher;

    if (epoll_ctl(loop->epoll_fd, EPOLL_CTL_ADD, watcher->fd, &ev) < 0) {
        return -1;
    }

    list_add(&loop->io_watchers, watcher);
    return 0;
}

void ev_io_stop(ev_loop_t *loop, ev_io_t *watcher)
{
    if (!loop || !watcher) {
        return;
    }

    epoll_ctl(loop->epoll_fd, EPOLL_CTL_DEL, watcher->fd, NULL);
    list_remove(&loop->io_watchers, watcher);
}

int ev_io_modify(ev_loop_t *loop, ev_io_t *watcher, int events)
{
    struct epoll_event ev;

    if (!loop || !watcher) {
        return -1;
    }

    watcher->events = events;

    ev.events = 0;
    if (events & EV_READ) {
        ev.events |= EPOLLIN;
    }
    if (events & EV_WRITE) {
        ev.events |= EPOLLOUT;
    }
    ev.data.ptr = watcher;

    return epoll_ctl(loop->epoll_fd, EPOLL_CTL_MOD, watcher->fd, &ev);
}
