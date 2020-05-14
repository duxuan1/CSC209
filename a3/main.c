#include <stdio.h> 
#include <float.h>
#include <stdlib.h> 
#include <math.h> 
#include <string.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>

#include <limits.h>

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "closest_parallel.h"

struct Point *generate_points(size_t n, char *filename) {
    struct Point *P;
    size_t n_written;
    int upper = RAND_MAX;
    int lower = 0;
    printf("Generating %ld random points with x- and y-coordinate range between %d and %d...",
            n, lower, upper);
    if (!(P = malloc(n*sizeof(struct Point)))) {
        perror("malloc");
        exit(1);
    }
    for (size_t i = 0; i < n; i++) {
        int x = rand();
        int y = rand();
        P[i].x = x;
        P[i].y = y;
        if (verbose) printf("%d %d\n", x, y);
    }
    if (filename) {
        FILE *f = fopen(filename, "wb");
        if (!f) {
            perror(filename);
            exit(1);
        }
        n_written = fwrite(&n, sizeof(n), 1, f);
        if (n_written != 1) {
            fprintf(stderr, "Error writing n to data file.\n");
            exit(1);
        }
        n_written = fwrite(P, sizeof(struct Point), n, f);
        if (n_written != n) {
            fprintf(stderr, "Error writing Point structs to data file.\n");
            exit(1);
        }
        if (fclose(f)) {
            fprintf(stderr, "Error closing data file.\n");
            exit(1);
        }
    }
    printf(" Done.\n");
    return P;
}

void print_usage(char *pname) {
    fprintf(stderr, "Usage: %s ", pname);
    fprintf(stderr, "[-g npoints] [-f filename] [-v] -d pdepth\n\n");

    fprintf(stderr, "    -d - Maximum process tree depth\n");
    fprintf(stderr, "    -g - Number of points to generate\n");
    fprintf(stderr, "    -f - File name to read or write points from\n");
    fprintf(stderr, "    -v - Verbose (print extra debugging info)");

    fprintf(stderr, "\n\n");

    fprintf(stderr, "Usage of optional parameters:\n\n");
    fprintf(stderr, "When -g and -f are not used, the points are read from the default file %s.\n\n", DEFAULT_FILE);
    fprintf(stderr, "When -f is used without -g, the points are read from the specified file.\n\n");
    fprintf(stderr, "When -g is used without -f, the specified number of points are generated at runtime but not saved.\n\n");
    fprintf(stderr, "When -g and -f are used together, the points are generated at runtime and stored in the specified file.\n\n");
    exit(1);
}

int main(int argc, char **argv) 
{
    if (argc == 1) print_usage(argv[0]);

    size_t num_read = 0; // Store ret. value from fread
    size_t n = 0; // # of Point structs to read from file
    // changed

    long opt, ngen = 0, pdepth = -1;
    char *filename = NULL;
    char *endptr;
    struct Point *P = NULL;
    int pcount = 0;
    
    opterr = 0; // So getopt() does not print errors

    while ((opt = getopt(argc, argv, "f:g:d:v")) != -1) {
        switch(opt) {
            case 'g':
                ngen = strtol(optarg, &endptr, 10);
                if ((errno == ERANGE && (ngen == LONG_MAX || ngen == LONG_MIN))
                        || (errno != 0 && ngen == 0)) {
                    perror("strtol");
                    exit(1);
                }
                if (endptr == optarg || ngen < 1 || *endptr != '\0') {
                    fprintf(stderr, "Option '-g' requires a positive integer argument.\n");
                    exit(1);
                }
                break;
            case 'd':
                pdepth = strtol(optarg, &endptr, 10);
                if ((errno == ERANGE && (pdepth == LONG_MAX || pdepth == LONG_MIN))
                        || (errno != 0 && pdepth == 0)) {
                    perror("strtol");
                    exit(1);
                }
                if (endptr == optarg || pdepth < 0 || *endptr != '\0') {
                    fprintf(stderr, "Option '-d' requires a non-negative integer argument.\n");
                    exit(1);
                }
                if (pdepth > 7) {
                    fprintf(stderr, "Warning: Process count will be inaccurate if chosen depth is greater than 7.\n");
                }
                break;
            case 'v':
                verbose = 1;
                break;
            case 'f':
                filename = optarg;
                break;
            case '?':
                if (optopt == 'g' || optopt == 'f' || optopt == 'd')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option '-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
                exit(1);
            default:
                print_usage(argv[0]);
        }
    }

    if (ngen > 0) {
        P = generate_points(ngen, filename);
        n = ngen;
    }
    else if (pdepth != -1) {
        char *f_name = (filename)? filename : DEFAULT_FILE;
        FILE *f = fopen(f_name, "rb");
        if (!f) {
            fprintf(stderr, "Failed to open file '%s'.\n", f_name);
            perror("fopen");
            exit(1);
        }
        num_read = fread(&n, sizeof(n), 1, f);
        if (num_read == 0) {
            fprintf(stderr, "Failed to read number of points from file '%s'.\n", f_name);
            exit(1);
        }
        printf("Reading %ld points from file '%s'...\n", n, f_name);

        if (!(P = malloc(sizeof(struct Point)*n))) {
            perror("malloc");
            exit(1);
        }

        num_read = fread(P, sizeof(struct Point), n, f);
        //printf("Read %d items\n\n", num_read);
        if (num_read != n) {
            fprintf(stderr, "Error reading from file '%s'.\n", f_name);
            exit(1);
        }
        if (fclose(f)) {
            fprintf(stderr, "Error closing from file '%s'.\n", f_name);
            exit(1);
        }
    }

    if (pdepth == -1) {
        fprintf(stdout, "Process depth not specified: Not running algorithm.\n");
        exit(0);
    }

    struct timespec start, end;
    double time_p, time_s, time_qs;

    if (clock_gettime(CLOCK_MONOTONIC, &start)) {
        perror("clock_gettime");
        exit(1);
    }
    qsort(P, n, sizeof(struct Point), compare_x);
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
    double result_p = _closest_parallel(P, n, pdepth, &pcount);
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
    double result_s = _closest_serial(P, n);
    if (clock_gettime(CLOCK_MONOTONIC, &end)) {
        perror("clock_gettime");
        exit(1);
    }
    time_s = 1e3*(double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_nsec - start.tv_nsec))/1e6;
    printf("[Single-Process] The smallest distance is %.2f (execution time: %.2fms).\n", result_s, time_s);

    free(P);
    exit(0);
} 
