#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
  #include <winsock2.h>
  #define write_socket(fd, buf, len) send((fd), (buf), (int)(len), 0)
#else
  #include <unistd.h>
  #define write_socket(fd, buf, len) write((fd), (buf), (len))
#endif

#include "response.h"
#include "server.h"

/* Map a file extension to a MIME type for the Content-Type header. */
static const char *mime_type(const char *path) {
    const char *dot = strrchr(path, '.');
    if (dot == NULL) {
        return "application/octet-stream";
    }
    if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0) return "text/html";
    if (strcmp(dot, ".css") == 0)  return "text/css";
    if (strcmp(dot, ".js") == 0)   return "application/javascript";
    if (strcmp(dot, ".json") == 0) return "application/json";
    if (strcmp(dot, ".png") == 0)  return "image/png";
    if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(dot, ".gif") == 0)  return "image/gif";
    if (strcmp(dot, ".svg") == 0)  return "image/svg+xml";
    if (strcmp(dot, ".ico") == 0)  return "image/x-icon";
    if (strcmp(dot, ".txt") == 0)  return "text/plain";
    return "application/octet-stream";
}

void send_error(int client_fd, int status_code, const char *status_text) {
    char body[256];
    char header[512];

    int body_len = snprintf(body, sizeof(body),
        "<html><head><title>%d %s</title></head>"
        "<body><h1>%d %s</h1></body></html>",
        status_code, status_text, status_code, status_text);

    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n",
        status_code, status_text, body_len);

    write_socket(client_fd, header, header_len);
    write_socket(client_fd, body, body_len);
}

/*
 * Reject paths that try to escape the web root via "..". This is a basic
 * but important guard against directory-traversal attacks.
 */
static int is_path_safe(const char *path) {
    return strstr(path, "..") == NULL;
}

void handle_request(int client_fd, const HttpRequest *req, const char *web_root) {
    /* Only GET is supported. */
    if (strcmp(req->method, "GET") != 0) {
        send_error(client_fd, 405, "Method Not Allowed");
        return;
    }

    if (!is_path_safe(req->path)) {
        send_error(client_fd, 403, "Forbidden");
        return;
    }

    /* Map "/" to the default document. */
    const char *url_path = req->path;
    if (strcmp(url_path, "/") == 0) {
        url_path = "/index.html";
    }

    char filepath[2048];
    snprintf(filepath, sizeof(filepath), "%s%s", web_root, url_path);

    FILE *fp = fopen(filepath, "rb");
    if (fp == NULL) {
        send_error(client_fd, 404, "Not Found");
        return;
    }

    /* Determine the file size for the Content-Length header. */
    struct stat st;
    if (stat(filepath, &st) != 0) {
        fclose(fp);
        send_error(client_fd, 404, "Not Found");
        return;
    }
    long content_length = (long)st.st_size;

    char header[512];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n"
        "\r\n",
        mime_type(filepath), content_length);

    write_socket(client_fd, header, header_len);

    /* Stream the file body in chunks. */
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        write_socket(client_fd, buffer, bytes_read);
    }

    fclose(fp);
}
