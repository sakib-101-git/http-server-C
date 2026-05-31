#ifndef RESPONSE_H
#define RESPONSE_H

#include "request.h"

/*
 * handle_request
 *   client_fd : connected client socket descriptor
 *   req       : the parsed request to fulfil
 *   web_root  : directory from which static files are served
 *
 * Writes a complete HTTP response (status line, headers, body) to the
 * client socket. Handles 200, 400, 403, 404 and 405 cases.
 */
void handle_request(int client_fd, const HttpRequest *req, const char *web_root);

/* Send a bodyless/short HTML error response with the given status. */
void send_error(int client_fd, int status_code, const char *status_text);

#endif /* RESPONSE_H */
