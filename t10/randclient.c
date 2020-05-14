#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "socket.h"

#ifndef SERVER_PORT
  #define SERVER_PORT 30000
#endif

#define MESSAGE "A stitch in time\r\n"
#define TIMES 5 // number of times to send the message
#define MINCHARS 3
#define MAXCHARS 7


void write_random_pieces(int soc, const char *message, int times);
int connect_to_server(int port, const char *hostname);

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: randclient hostname\n");
        exit(1);
    }

    int soc = connect_to_server(SERVER_PORT, argv[1]);

    write_random_pieces(soc, MESSAGE, TIMES);

    close(soc);
    return 0;
}


/*
 * Write the given message the given number of times to the given socket, and randomly-sized pieces.
 * Do NOT write any null-terminator characters.
 */
void write_random_pieces(int soc, const char *message, int times) {
    char piece[MAXCHARS];
    int message_len = strlen(message);
    int total_bytes = times * message_len;
    int current_byte = 0;

    while (current_byte < total_bytes) {
        int piece_size = rand() % (MAXCHARS - MINCHARS + 1) + MINCHARS;
        int bytes_left = total_bytes - current_byte;
        if (piece_size > bytes_left) {
            piece_size = bytes_left;
        }

        for (int i = 0; i < piece_size; i++) {
            piece[i] = message[(current_byte + i) % message_len];
        }
        write(soc, piece, piece_size);
        current_byte += piece_size;
    }
}

/*
 * Create a socket and connect to the server indicated by the port and hostname
 */
int connect_to_server(int port, const char *hostname) {
    int soc = socket(PF_INET, SOCK_STREAM, 0);
    if (soc < 0) {
        perror("socket");
        exit(1);
    }
    struct sockaddr_in addr;

    // Allow sockets across machines.
    addr.sin_family = PF_INET;
    // The port the server will be listening on.
    addr.sin_port = htons(port);
    // Clear this field; sin_zero is used for padding for the struct.
    memset(&(addr.sin_zero), 0, 8);

    // Lookup host IP address.
    struct hostent *hp = gethostbyname(hostname);
    if (hp == NULL) {
        fprintf(stderr, "unknown host %s\n", hostname);
        exit(1);
    }

    addr.sin_addr = *((struct in_addr *) hp->h_addr);

    // Request connection to server.
    if (connect(soc, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        exit(1);
    }

    return soc;
}
