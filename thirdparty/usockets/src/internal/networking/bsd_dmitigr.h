#ifndef BSD_DMITIGR_H
#define BSD_DMITIGR_H

#include "bsd.h"

static inline int bsd_socket_local_addr(LIBUS_SOCKET_DESCRIPTOR fd, struct bsd_addr_t *addr) {
    addr->len = sizeof(addr->mem);
    if (getsockname(fd, (struct sockaddr *) &addr->mem, &addr->len)) {
        return -1;
    }
    internal_finalize_bsd_addr(addr);
    return 0;
}

#endif  /* BSD_DMITIGR_H */
