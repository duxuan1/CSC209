#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "socket.h"

/*
 * Wait for and accept a new connection.
 * Return -1 if the accept call failed.
 */
int accept_connection(int listenfd) {
    struct sockaddr_in peer;
    unsigned int peer_len = sizeof(peer);
    peer.sin_family = PF_INET;

    fprintf(stderr, "Waiting for a new connection...\n");
    int client_socket = accept(listenfd, (struct sockaddr *)&peer, &peer_len);
    if (client_socket < 0) {
        perror("accept");
        return -1;
    } else {
        fprintf(stderr,
            "New connection accepted from %s:%d\n",
            inet_ntoa(peer.sin_addr),
            ntohs(peer.sin_port));
        return client_socket;
    }
}

int main() {
    struct listen_sock s;
    setup_server_socket(&s);

    while (1) {
        int fd = accept_connection(s.sock_fd);
        if (fd < 0) {
            continue;
        }

        // Receive messages
        char buf[BUF_SIZE];
        int nbytes;
        while ((nbytes = read(fd, buf, sizeof(buf) - 1)) > 0) {
            buf[nbytes] = '\0';
            printf("Next message: %s\n", buf);
        }
        close(fd);
    }

    free(s.addr);
    close(s.sock_fd);
    return 0;
}
