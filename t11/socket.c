#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>     /* inet_ntoa */
#include <netdb.h>         /* gethostname */
#include <netinet/in.h>    /* struct sockaddr_in */

#include "socket.h"

void setup_server_socket(struct listen_sock *s) {
    if(!(s->addr = malloc(sizeof(struct sockaddr_in)))) {
        perror("malloc");
        exit(1);
    }
    // Allow sockets across machines.
    s->addr->sin_family = AF_INET;
    // The port the process will listen on.
    s->addr->sin_port = htons(SERVER_PORT);
    // Clear this field; sin_zero is used for padding for the struct.
    memset(&(s->addr->sin_zero), 0, 8);
    // Listen on all network interfaces.
    s->addr->sin_addr.s_addr = INADDR_ANY;

    s->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s->sock_fd < 0) {
        perror("server socket");
        exit(1);
    }

    // Make sure we can reuse the port immediately after the
    // server terminates. Avoids the "address in use" error
    int on = 1;
    int status = setsockopt(s->sock_fd, SOL_SOCKET, SO_REUSEADDR,
        (const char *) &on, sizeof(on));
    if (status < 0) {
        perror("setsockopt");
        exit(1);
    }

    // Bind the selected port to the socket.
    if (bind(s->sock_fd, (struct sockaddr *)s->addr, sizeof(*(s->addr))) < 0) {
        perror("server: bind");
        close(s->sock_fd);
        exit(1);
    }

    // Announce willingness to accept connections on this socket.
    if (listen(s->sock_fd, MAX_BACKLOG) < 0) {
        perror("server: listen");
        close(s->sock_fd);
        exit(1);
    }
}

/* Insert Tutorial 10 helper functions here. */

int find_network_newline(const char *buf, int inbuf) {
    for (int i = 0; i <= inbuf; i++){
        if (buf[i] == '\r' && i < inbuf && buf[i + 1] == '\n'){
            return i + 2;
        }
    }
    return -1;
}

int read_from_socket(int sock_fd, char *buf, int *inbuf) {
    if (sock_fd < 0) {
        return 1;
    }
    int num_read = read(sock_fd, buf + *inbuf, BUF_SIZE - *inbuf);
    if (num_read == -1 || num_read == 0) {
        return num_read;
    }
    *inbuf += num_read;
    int new_line = find_network_newline(buf, *inbuf);
    if (new_line == -1 && *inbuf > BUF_SIZE) {
        return -1;
    } else if (new_line > 0) {
        return 0;
    }
    return 2;
}

int get_message(char **dst, char *src, int *inbuf) {
    int end = find_network_newline(src, *inbuf);
    if (end != -1) {
        *dst = malloc(BUF_SIZE * sizeof(char));
        strncpy(*dst, src, end - 2);
        (*dst)[end - 2] = '\0';
        *inbuf -= end;
        memmove(src, src + end, *inbuf);
        return 0;
    } 
    return 1;
}

/* Helper function to be completed for Tutorial 11. */

int write_to_socket(int sock_fd, char *buf, int len) {
    if (sock_fd < 0) {
        return 2;
    }
    if (write(sock_fd, buf, len) < 0) {
        return 1;
    }
    return 0;
}
