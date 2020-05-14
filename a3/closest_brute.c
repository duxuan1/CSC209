#include "closest_brute.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

double brute_force(struct Point P[], size_t n) { 
    int size = n;
    double min_dis = INFINITY;
    for (int i = 0; i < size - 1; i++) {
        for (int j = i + 1; j < size; j++) {
            double dis = dist(P[i], P[j]);
            if (dis < min_dis) {
                min_dis = dis;
            }
        }
    }
    return min_dis; 
} 