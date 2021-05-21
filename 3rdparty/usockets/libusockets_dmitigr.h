/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

 *     http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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
