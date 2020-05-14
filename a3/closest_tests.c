#include <stdio.h> 
#include <float.h>
#include <stdlib.h> 
#include <math.h> 
#include <time.h>

#include <limits.h>

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

#include "closest_parallel.h"

int main(int argc, char **argv) 
{
    struct Point P1[] = {{2, 3}, {12, 30}, {40, 50}, {5, 1}, {12, 10}, {3, 4}, {22, 13}};
    int n1 = sizeof(P1) / sizeof(P1[0]);
    int pdepth = 7, pcount = 0;

    struct timespec start, end;
    double time_p, time_s, time_qs;

    printf("--- Starting Test #1 ---\n");
    if (clock_gettime(CLOCK_MONOTONIC, &start)) {
        perror("clock_gettime");
        exit(1);
    }
    qsort(P1, n1, sizeof(struct Point), compare_x);
    if (clock_gettime(CLOCK_MONOTONIC, &end)) {
        perror("clock_gettime");
        exit(1);
    }
    time_qs = 1e3*(double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_nsec - start.tv_nsec))/1e6;
    printf("Sorted the points in %.2fms.\n", time_qs); 

    if (clock_gettime(CLOCK_MONOTONIC, &start)) {
        perror("clock_gettime");
        exit(1);
    }
    double result_p = _closest_parallel(P1, n1, pdepth, &pcount);
    if (clock_gettime(CLOCK_MONOTONIC, &end)) {
        perror("clock_gettime");
        exit(1);
    }
    time_p = 1e3*(double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_nsec - start.tv_nsec))/1e6;
    printf("[Multi-Process] The smallest distance is %.2f", result_p);
    printf(" (execution time: %.2fms; processes created: %d).\n", time_p, pcount); 

    if (clock_gettime(CLOCK_MONOTONIC, &start)) {
        perror("clock_gettime");
        exit(1);
    }
    double result_s = _closest_serial(P1, n1);
    if (clock_gettime(CLOCK_MONOTONIC, &end)) {
        perror("clock_gettime");
        exit(1);
    }
    time_s = 1e3*(double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_nsec - start.tv_nsec))/1e6;
    printf("[Single-Process] The smallest distance is %.2f (execution time: %.2fms).\n", result_s, time_s);

    printf("Expected result: 1.41\n");
    printf("--- End of Test #1 ---\n");

    exit(0);
} 
