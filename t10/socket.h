#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <netinet/in.h>    /* Internet domain header, for struct sockaddr_in */

#ifndef SERVER_PORT
  #define SERVER_PORT 30000
#endif
#define MAX_BACKLOG 5
#define BUF_SIZE 30

struct listen_sock {
    struct sockaddr_in *addr;
    int sock_fd;
};

void setup_server_socket(struct listen_sock *s);
int find_network_newline(const char *buf, int n);
int read_from_socket(int sock_fd, char *buf, int *inbuf);
int get_message(char **dst, char *src, int *inbuf);

#endif
