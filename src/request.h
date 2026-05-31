#ifndef REQUEST_H
#define REQUEST_H

#define METHOD_SIZE 16
#define PATH_SIZE 1024
#define VERSION_SIZE 16

/* A minimal parsed HTTP request line: "METHOD PATH VERSION". */
typedef struct {
    char method[METHOD_SIZE];   /* e.g. "GET" */
    char path[PATH_SIZE];       /* e.g. "/index.html" */
    char version[VERSION_SIZE]; /* e.g. "HTTP/1.1" */
} HttpRequest;

/*
 * parse_request
 *   raw : the raw bytes read from the client socket
 *   req : output struct populated on success
 *
 * Returns 0 on success, -1 if the request line could not be parsed.
 */
int parse_request(const char *raw, HttpRequest *req);

#endif /* REQUEST_H */
