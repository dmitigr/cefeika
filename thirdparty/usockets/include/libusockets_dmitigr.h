#include "libusockets.h"

#ifndef LIBUSOCKETS_DMITIGR_H
#define LIBUSOCKETS_DMITIGR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Copy local (IP) address of socket, or fail with zero length. */
WIN32_EXPORT void us_socket_local_address(int ssl, struct us_socket_t *s, char *buf, int *length);

#ifdef __cplusplus
}
#endif

#endif  /* LIBUSOCKETS_DMITIGR_H */
