#include <stdio.h>
#include <string.h>

#include "request.h"

/*
 * Parse only the first line of an HTTP request, which has the form:
 *
 *     METHOD SP REQUEST-URI SP HTTP-VERSION CRLF
 *
 * Everything after the request line (headers, body) is ignored, which is
 * enough for a simple static file server.
 */
int parse_request(const char *raw, HttpRequest *req) {
    if (raw == NULL || req == NULL) {
        return -1;
    }

    /* sscanf with width limits guards against buffer overflows. */
    int matched = sscanf(raw, "%15s %1023s %15s",
                         req->method, req->path, req->version);

    if (matched != 3) {
        return -1;
    }

    return 0;
}
