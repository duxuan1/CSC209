#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#include "socket.h"

struct client_sock {
    int sock_fd;
    char buf[BUF_SIZE];
    int consumed;
    int inbuf;
};

int sigint_received = 0;

void sigint_handler(int code) {
    sigint_received = 1;
}

/*
 * Wait for and accept a new connection.
 * Return -1 if the accept call failed.
 */
int accept_connection(int fd, struct client_sock **client) {
    struct sockaddr_in peer;
    unsigned int peer_len = sizeof(peer);
    peer.sin_family = AF_INET;
    
    fprintf(stderr, "Waiting for a new connection...\n");
    int client_fd = accept(fd, (struct sockaddr *)&peer, &peer_len);
    if (client_fd < 0) {
        perror("accept");
        return -1;
    }
    fprintf(stderr,
        "New connection accepted from %s:%d\n",
        inet_ntoa(peer.sin_addr),
        ntohs(peer.sin_port));

    *client = malloc(sizeof(struct client_sock));
    (*client)->sock_fd = client_fd;
    (*client)->consumed = (*client)->inbuf = 0;
    memset((*client)->buf, 0, BUF_SIZE);
    
    return client_fd;
}

int read_from_client(struct client_sock *curr) {
    return read_from_socket(curr->sock_fd, curr->buf, &(curr->inbuf));
}

/*
 * Close all sockets, free memory, and exit with specified exit status.
 */
void clean_exit(struct listen_sock s, struct client_sock *client, int exit_status) {
    if (client) {
        close(client->sock_fd);
        free(client);
    }
    close(s.sock_fd);
    free(s.addr);
    exit(exit_status);
}

int main() {
    // This line causes stdout not to be buffered.
    // Don't change this! Necessary for autotesting.
    setbuf(stdout, NULL);
    
    // Next week this will be a linked list of clients.
    // For now we will just support 1 connected client
    // at any given time.
    struct client_sock *clients = NULL;
    
    struct listen_sock s;
    setup_server_socket(&s);
    
    // Set up SIGINT handler. We want the server to terminate
    // gracefully upon receiving a SIGINT. Minimally, this would
    // involve closing all TCP sockets. But if we wanted to be fancy
    // we could even send a message to all clients letting them know
    // that the server is shutting down. We will also need to free
    // any allocated memory, to avoid upsetting valgrind or Leak Sanitizer.
    struct sigaction sa_sigint;
    memset (&sa_sigint, 0, sizeof (sa_sigint));
    sa_sigint.sa_handler = sigint_handler;
    sa_sigint.sa_flags = 0;
    sigemptyset(&sa_sigint.sa_mask);
    sigaction(SIGINT, &sa_sigint, NULL);
    
    int exit_status = 0;

    do {
        int fd = accept_connection(s.sock_fd, &clients);
        if (sigint_received) break;
        if (fd < 0) {
            fprintf(stderr, "Failed to accept incoming connection.\n");
            continue;
        }

        // Receive messages
        int client_closed;
        do {
            // Step 1: Receive data from client and save into buffer
            // Implement read_from_client() in socket.c
            client_closed = read_from_client(clients);
            if (client_closed == -1) { // Read error
                fprintf(stderr, "Error receiving from client.\n");
                clean_exit(s, clients, 1);
            }
            else if (client_closed == 1) { // Client disconnected
                close(fd);
                free(clients);
                clients = NULL;
                fprintf(stderr, "Client disconnected.\n");
            }
            else if (client_closed == 0) { // Received CRLF
                char *msg;
                // Step 2: Extract each CRLF-terminated message
                // from the buffer into a NULL-terminated string.
                // Free the string once we're done with it.
                // Implement get_message() in socket.c
                while (!get_message(&msg, clients->buf, &(clients->inbuf))) {
                    printf("Next message: %s\n", msg);
                    free(msg);
                }
            }
        } while(client_closed != 1); // Loop as long as client is connected
    } while(!sigint_received); // Loop as long as no SIGINT received
    
    clean_exit(s, clients, exit_status);
}
