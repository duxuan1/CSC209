#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256
#define MAX_PASSWORD 10

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

int main(void) {
    char user_id[MAXLINE];
    char password[MAXLINE];

    if(fgets(user_id, MAXLINE, stdin) == NULL) {
        perror("fgets");
        exit(1);
    }
    if(fgets(password, MAXLINE, stdin) == NULL) {
        perror("fgets");
        exit(1);
    }

    // create pipe
    int p[2];
    if (pipe(p) == -1) {
        perror("pipe");
    }
    int r = fork();
    if (r < 0) { // no process created
        perror("fork");
        exit(1);
    } else if (r == 0) { // child process
        if ((close(p[1])) == -1) {
            perror("close");
            exit(1);
        }
        if (dup2(p[0], fileno(stdin)) == -1) {
			perror("dup2");
		}
        if (execl("./validate", "./validate", NULL) == -1) {
            perror("execl");
            exit(1);
        }
		perror("exec");
		exit(1);
    } else if (r > 0) { // parent process
        if ((close(p[0])) == -1) {
            perror("close");
            exit(1);
        }
        if (write(p[1], user_id, MAX_PASSWORD) == -1) {
            perror("write");
            exit(1);
        }
        if (write(p[1], password, MAX_PASSWORD) == -1) {
            perror("write");
            exit(1);
        }
        if ((close(p[1])) == -1) {
            perror("close");
            exit(1);
        }
        int status;
        wait(&status);
		int e = WEXITSTATUS(status);
        if (e == 0) {
            printf(SUCCESS);
        } else if (e == 2) {
            printf(INVALID);
        } else {
            printf(NO_USER);
        }
    }
    return 0;
}
