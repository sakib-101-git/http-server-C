#ifndef SERVER_H
#define SERVER_H

/* Default values used when none are supplied on the command line. */
#define DEFAULT_PORT 8080
#define DEFAULT_ROOT "www"
#define BACKLOG 16          /* pending connection queue length */
#define BUFFER_SIZE 8192    /* size of the per-request read buffer */

/*
 * start_server
 *   port     : TCP port to listen on
 *   web_root : directory whose files are served
 *
 * Blocks forever, accepting and handling connections one at a time.
 * Returns a non-zero value only if the server fails to start.
 */
int start_server(int port, const char *web_root);

#endif /* SERVER_H */
