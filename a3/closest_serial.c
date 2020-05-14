#include "closest_serial.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <math.h>

double combine_lr(struct Point P[], size_t n, struct Point mid_point, double d) {
    // find the size of array need to be created
    int total = 0, count = 0;
    struct Point points[n];
    // create array
    for (int i = 0; i < n; i++) {
        if (abs(P[i].x - mid_point.x) < d) {
            points[count] = P[i];
            count++;
            total++;
        }
    }
    // brute force and find minimun distance through y-sorted array
    qsort((void*)points, total, sizeof(struct Point), compare_y); 
    double min_dis = INFINITY;
    for (int i = 0; i < total - 1; i++) {
        for (int j = i + 1; j < total; j++) {
            double dis = dist(points[i], points[j]);
            if (dis < min_dis) {
                min_dis = dis;
            }
        }
    }
    return fmin(d, min_dis);
}

double _closest_serial(struct Point P[], size_t n) {
    // terminating if size of P is less than 3, use brute forse
    if (n <= 3) {
        return brute_force(P, n);
    }
    // divide P in two parts
    int p_mid = floor(n/2);
    int sizeleft = p_mid, sizeright = n - p_mid;
    double dl, dr, dy, d;
    // rescurivly function call
    dl = _closest_serial(P, sizeleft);
    dr = _closest_serial(&P[p_mid], sizeright);
    d = fmin(dl, dr);
    // brute force through y-sorted P
    dy = combine_lr(P, n, P[p_mid], d);
    return dy;
}

double closest_serial(struct Point P[], size_t n) {
    qsort((void*)P, n, sizeof(struct Point), compare_x); 
    return _closest_serial(P, n);
}