#include "libusockets_dmitigr.h"
#include "internal/internal_dmitigr.h"
#include <stdlib.h>

void us_socket_local_address(int ssl, struct us_socket_t *s, char *buf, int *length) {
    struct bsd_addr_t addr;
    if (bsd_socket_local_addr(us_poll_fd(&s->p), &addr) || *length < bsd_addr_get_ip_length(&addr)) {
        *length = 0;
    } else {
        *length = bsd_addr_get_ip_length(&addr);
        memcpy(buf, bsd_addr_get_ip(&addr), *length);
    }
}
