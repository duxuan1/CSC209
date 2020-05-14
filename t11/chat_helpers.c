#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>

#include "socket.h"
#include "chat_helpers.h"

int write_buf_to_client(struct client_sock *c, char *buf, int len) {
    if (c->sock_fd < 0) {
        return 2;
    }
    for (int i = 0; i <= len; i++) {
        if (buf[i] == '\0') {
            buf[i] = '\r';
            buf[i + 1] = '\n';
            return write_to_socket(c->sock_fd, buf, len + 2);
        }
    }
    return 1;
}

int remove_client(struct client_sock **curr, struct client_sock **clients) {
    // check head
    struct client_sock *tmp;
    if (strcmp((*clients)->username, (*curr)->username) == 0) {
        tmp = (*clients);
        (*clients) = (*clients)->next;
        free(tmp->username);
        free(tmp);
        return 0;
    }
    // loop through ll to find client
    while (*clients) {
        tmp = *curr;
        if (strcmp(tmp->username, (*curr)->username) == 0) {
            (*curr) = (*curr)->next;
            free(tmp->username);
            free(tmp);
            return 0; // found client in the list 
        }
        (*clients) = (*clients)->next;
    }
    return 1; // Couldn't find the client in the list, or empty list
}

int read_from_client(struct client_sock *curr) {
    return read_from_socket(curr->sock_fd, curr->buf, &(curr->inbuf));
}

int set_username(struct client_sock *curr) {
    // To be completed. Hint: Use get_message().
    char *msg;
    // get message from socket 
    if (!get_message(&msg, curr->buf, &(curr->inbuf))) {
        curr->username = msg;
        return 0;
    }
    return 1; // failed to get message
}
