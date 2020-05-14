#ifndef CLOSEST_SERIAL_H
#define CLOSEST_SERIAL_H

#include "closest_brute.h"

/*
 * Find the distance between closest pair of points within a vertical
 * strip centered at the midpoint, where both points are within
 * distance d from the vertical line passing through the midpoint.
 */
double combine_lr(struct Point P[], size_t n, struct Point midPoint, double d);

/*
 * Single-process recursive divide-and-conquer implementation to find the
 * distance between the closest pair of n points in P[]. Assumes that the
 * array P[] is sorted by x-coordinate.
 */
double _closest_serial(struct Point P[], size_t n);

/*
 * Wrapper function that sorts P[] and then calls _closest_serial().
 * Use this function if P[] is not pre-sorted.
 */
double closest_serial(struct Point P[], size_t n);
    
#endif /* CLOSEST_SERIAL_H */
