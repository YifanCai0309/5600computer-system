/*
 * mpg2km.c / Assignment 1 - Conversion Functions
 *
 * Contains functions for converting mileage/efficiency metrics.
 *
 * Yifan Cai / CS5600 / Northeastern University
 * Fall 2024 / Sep 24, 2024
 *
 */

#include "mpg2km.h"

// Convert miles per gallon to kilometers per liter
double mpg2kml(double mpg) {
    if (mpg <= 0) return -1;  // Handle invalid cases
    return mpg * 1.609344 / 3.785411784;  // Conversion factor: 1 mile = 1.609344 km, 1 gallon = 3.78541 liters
}

// Convert miles per gallon to liters per 100 kilometers
double mpg2lphm(double mpg) {
    if (mpg <= 0) return -1;  // Handle invalid cases
    return 100.0 / (mpg2kml(mpg));  // Conversion using the previous function
}

// Convert liters per 100 kilometers to miles per gallon
double lph2mpg(double lph) {
    if (lph <= 0) return -1;  // Handle invalid cases
    return 235.214583 / lph;  // Conversion factor: 235.214583 miles/gallon
}
