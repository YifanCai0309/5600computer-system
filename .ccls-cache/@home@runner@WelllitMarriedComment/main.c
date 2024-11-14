/*
 * learnc.c / Assignment 1
 *
 * Yifan Cai / CS5600 / Northeastern University
 * Fall 2024 / Sep 24, 2024
 *
 */

#include <stdio.h>
#include "mpg2km.h"  // Include the header file with function signatures

int main() {
    // Test cases for the functions
    double mpg = 25.0;
    double lph = 8.5;

    // Test valid inputs
    printf("Testing mpg2kml(25.0): %f km/l\n", mpg2kml(mpg));
    printf("Testing mpg2lphm(25.0): %f liters/100km\n", mpg2lphm(mpg));
    printf("Testing lph2mpg(8.5): %f mpg\n", lph2mpg(lph));

    // Test invalid/bad arguments
    printf("Testing mpg2kml(-10.0): %f km/l (expected failure case)\n", mpg2kml(-10.0));
    printf("Testing mpg2lphm(0.0): %f liters/100km (expected failure case)\n", mpg2lphm(0.0));

    return 0;
}
