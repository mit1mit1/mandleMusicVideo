#ifndef MATH_H    // To make sure you don't declare the function more than once by including the header multiple times.
#define MATH_H

#include "structs.h"
#include <vector>

// #include "stdafx.h"
// #include <SFML/Graphics.hpp>

int Mandelbrot(double cr, double ci, int limit);
Coordinate GetInterestingPoint(int mandleCounts[][yResolution], double xStepDistance, double yStepDistance, double centreX, double centreY);
int getPitchSum(std::vector<AubioNote> notes);

#endif