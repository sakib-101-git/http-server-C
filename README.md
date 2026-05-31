# HTTP Server in C

A small static-file HTTP/1.1 server written in C, supporting both POSIX
(Linux/macOS) and Windows (Winsock).

## Features

- Serves static files from a configurable web root (`www/` by default)
- Handles `GET` requests with correct `Content-Type` and `Content-Length`
- Common MIME types (HTML, CSS, JS, JSON, images, plain text)
- Basic error responses: `400`, `403`, `404`, `405`
- Directory-traversal protection (rejects paths containing `..`)

## Project structure

```
http-server-c/
├── src/
│   ├── main.c        # entry point, argument parsing
│   ├── server.c      # socket setup and the accept loop
│   ├── server.h
│   ├── request.c     # parses the HTTP request line
│   ├── request.h
│   ├── response.c    # builds responses, serves files
│   └── response.h
├── www/
│   └── index.html    # files the server will serve
├── Makefile
└── README.md
```

## Build

Requires `gcc` and `make`.

```sh
make
```

On Windows, use a toolchain that provides `gcc` and `make` (e.g. MinGW-w64
or MSYS2). The Makefile links Winsock (`-lws2_32`) automatically.

## Run

```sh
make run                 # default: port 8080, root ./www
./http-server            # same as above
./http-server 3000       # custom port
./http-server 3000 site  # custom port and web root
```

Then open <http://localhost:8080/> in your browser.

## Clean

```sh
make clean
```

## Notes & limitations

This is an educational project. It handles one connection at a time
(no threading), supports only `GET`, and reads just the request line.
It is **not** intended for production use.
