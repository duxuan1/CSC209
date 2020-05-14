#ifndef CLOSEST_PARALLEL_H
#define CLOSEST_PARALLEL_H

#include "closest_serial.h"

/* Global variable to track depth in the process tree */
extern int curr_depth;

/*
 * Multi-process parallel implementation of the recursive divide-and-conquer
 * algorithm to find the distance between the closest pair of n points in P[].
 * Assumes that P[] is sorted by x-coordinate.
 */
double _closest_parallel(struct Point P[], size_t n, int pdmax, int *pcount);

/*
 * Wrapper function that sorts P[] and then calls _closest_parallel().
 * Use this function if P[] is not pre-sorted
 */
double closest_parallel(struct Point P[], size_t n, int pdmax, int *pcount);

#endif /* CLOSEST_PARALLEL_H */
