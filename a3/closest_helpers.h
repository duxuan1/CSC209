#ifndef CLOSEST_HELPERS_H
#define CLOSEST_HELPERS_H

#define DEFAULT_FILE "points.dat"

/* Global variable to control output verbosity */
extern int verbose;

/* A struct representing a Point on 2D plane */
struct Point
{
    int x, y;
};

/* 
 * The following two functions are needed for library function qsort().
 * See "man 3 qsort" for details.
 *
 * The two arguments point to the two Point structs being compared.
 * 
 * <0 The element pointed by p1 goes before the element pointed by p2
 * =0 The element pointed by p1 is equivalent to the element pointed by p2
 * >0 The element pointed by p1 goes after the element pointed by p2
 */
int compare_x(const void*, const void*); // To sort by x-coord
int compare_y(const void*, const void*); // To sort by y-coord

/*
 * Calculate distance between two Point structs.
 *
 * Warning: You must avoid signed integer overflow when
 * calculating distance.
 */
double dist(struct Point, struct Point);

#endif /* CLOSEST_HELPERS_H */
