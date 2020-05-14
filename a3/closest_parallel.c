#include "closest_parallel.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <math.h>

int curr_depth = 0;

double _closest_parallel(struct Point P[], size_t n, int pdmax, int *pcount) {
    static int num_forks = 0;
    if (n < 4 || pdmax == 0) {
        return _closest_serial(P, n);
    } 
    // divide P in two parts
    int p_mid = floor(n/2);
    int sizeleft = p_mid, sizeright = n - p_mid;
    // create pipe for left side
    int p1[2];
    if (pipe(p1) < 0) { // fail to create process
        perror("pipe");
        exit(1);
    }
    int l = fork();
    if (l < 0) { // fail to create process
        perror("pipe");
        exit(1);
    } else if (l == 0) { // child process
        // closing read
        if ((close(p1[0])) == -1) {
            perror("close");
            exit(1);
        }
        // Send the distance between the closest pair of points back to the parent process
        double left_min = _closest_parallel(P, sizeleft, pdmax - 1, pcount);
        if (write(p1[1], &left_min, sizeof(double)) == -1) {
            perror("write");
            exit(1);
        }
        // closing writing p1
        if ((close(p1[1])) == -1) {
            perror("close");
            exit(1);
        }
        exit(num_forks);
    }
    // create pipe for right side
    int p2[2];
    if (pipe(p2) < 0) { // fail to create process
        perror("pipe");
        exit(1);
    }
    int r = fork();
    if (r < 0) { // fail to create process
        perror("pipe");
        exit(1);
    }  else if (r == 0) { // child process
        // closing read
        if ((close(p2[0])) == -1) {
            perror("close");
            exit(1);
        }
        // Send the distance between the closest pair of points back to the parent process
        double right_min = _closest_parallel(&(P[p_mid]), sizeright, pdmax - 1, pcount);
        if (write(p2[1], &right_min, sizeof(double)) == -1) {
            perror("write");
            exit(1);
        }
        // closing writing p2
        if ((close(p2[1])) == -1) {
            perror("close");
            exit(1);
        }
        exit(num_forks);
    } 
    // parent process for both
    if (l > 0 && r > 0) {
        double left_min, right_min;;
        for (int i = 0; i < 2; i++) {
            int status;
            wait(&status);
            int e = WEXITSTATUS(status);
            if (e != -1) {
                // two fork will be created
                curr_depth += 1;
                num_forks += e + 1;
            }
        } 
        // closing writing p1
        if ((close(p1[1])) == -1) {
            perror("close");
            exit(1);
        }
        // closing writing p2
        if ((close(p2[1])) == -1) {
            perror("close");
            exit(1);
        }
        // read from pipe1
        if (read(p1[0], &left_min, sizeof(double)) == -1) {
            perror("read");
            exit(1);
        }
        // read from pipe2
        if (read(p2[0], &right_min, sizeof(double)) == -1) {
            perror("read");
            exit(1);
        }
        // closing read
        if ((close(p1[0])) == -1) {
            perror("close");
            exit(1);
        }
        // closing read
        if ((close(p2[0])) == -1) {
            perror("close");
            exit(1);
        }
        *pcount = num_forks;
        double d = fmin(left_min, right_min);
        double dy = combine_lr(P, n, P[p_mid], d);
        return dy;
    }
    return 0.0;
}

double closest_parallel(struct Point P[], size_t n, int pdmax, int *pcount) {
    qsort((void*)P, n, sizeof(struct Point), compare_x); 
    return _closest_parallel(P, n, pdmax, pcount);
}