#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #pragma comment(lib, "ws2_32.lib")
  typedef int socklen_t;
  #define close_socket closesocket
#else
  #include <unistd.h>
  #include <arpa/inet.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #define close_socket close
#endif

#include "server.h"
#include "request.h"
#include "response.h"

/* On Windows, Winsock must be initialised before any socket call. */
static int net_init(void) {
#ifdef _WIN32
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 2), &wsa);
#else
    return 0;
#endif
}

static void net_cleanup(void) {
#ifdef _WIN32
    WSACleanup();
#endif
}

int start_server(int port, const char *web_root) {
    if (net_init() != 0) {
        fprintf(stderr, "Failed to initialise networking\n");
        return 1;
    }

    int server_fd = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        net_cleanup();
        return 1;
    }

    /* Allow quick restarts without "Address already in use". */
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
               (const char *)&opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons((unsigned short)port);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close_socket(server_fd);
        net_cleanup();
        return 1;
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        close_socket(server_fd);
        net_cleanup();
        return 1;
    }

    /* Accept loop: handle one connection at a time. */
    for (;;) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = (int)accept(server_fd,
                                    (struct sockaddr *)&client_addr,
                                    &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        char buffer[BUFFER_SIZE];
        int received = (int)recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (received > 0) {
            buffer[received] = '\0';

            HttpRequest req;
            if (parse_request(buffer, &req) == 0) {
                printf("%s %s\n", req.method, req.path);
                handle_request(client_fd, &req, web_root);
            } else {
                send_error(client_fd, 400, "Bad Request");
            }
        }

        close_socket(client_fd);
    }

    close_socket(server_fd);
    net_cleanup();
    return 0;
}
