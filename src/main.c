#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"

/*
 * Entry point.
 *
 * Usage: http-server [port] [web_root]
 *   port     defaults to DEFAULT_PORT (8080)
 *   web_root defaults to DEFAULT_ROOT ("www")
 */
int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;
    const char *web_root = DEFAULT_ROOT;

    if (argc > 1) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Invalid port: %s\n", argv[1]);
            return EXIT_FAILURE;
        }
    }
    if (argc > 2) {
        web_root = argv[2];
    }

    printf("Starting HTTP server on port %d, serving '%s'\n", port, web_root);
    printf("Open http://localhost:%d/ in your browser. Ctrl+C to stop.\n", port);

    return start_server(port, web_root) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
