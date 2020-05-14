#include "closest_helpers.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

int verbose = 0;

/* 
 * <0 The element pointed by p1 goes before the element pointed by p2
 * =0 The element pointed by p1 is equivalent to the element pointed by p2
 * >0 The element pointed by p1 goes after the element pointed by p2
 */
int compare_x(const void* a, const void* b) {
    int first = ((struct Point *)a) -> x; 
    int second = ((struct Point *)b) -> x;  
    return first - second;
} 

int compare_y(const void* a, const void* b) { 
    int first = ((struct Point *)a) -> y; 
    int second = ((struct Point *)b) -> y;  
    return first - second;
} 

double dist(struct Point p1, struct Point p2) {   
    double xdis = pow((abs(p1.x - p2.x)), 2);
    double ydis = pow((abs(p1.y - p2.y)), 2);
    double dis = sqrt(xdis + ydis);
    return dis;
} 