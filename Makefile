# Makefile for the C HTTP server
#
#   make        build the server
#   make run    build, then run on the default port (8080)
#   make clean  remove build artifacts

CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -O2
SRC_DIR := src
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(SOURCES:.c=.o)
TARGET  := http-server

# Link against Winsock when building on Windows.
ifeq ($(OS),Windows_NT)
    LDLIBS  := -lws2_32
    TARGET  := http-server.exe
endif

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

# Compile each .c into a .o; headers trigger a rebuild via the wildcard dep.
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(wildcard $(SRC_DIR)/*.h)
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)
