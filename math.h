#ifndef MATH_H // To make sure you don't declare the function more than once by including the header multiple times.
#define MATH_H

#include "structs.h"
#include <vector>

// #include "stdafx.h"
// #include <SFML/Graphics.hpp>

int Mandelbrot(long double cr, long double ci, int limit);
Coordinate chooseClosestInterestingPoint(std::vector<PixelIndex> interestingPoints,
                                         long double xStepDistance,
                                         long double yStepDistance,
                                         long double centreX,
                                         long double centreY,
                                         long double targetX,
                                         long double targetY);
Coordinate chooseRandomInterestingPoint(std::vector<PixelIndex> interestingPoints,
                                        long double xStepDistance,
                                        long double yStepDistance,
                                        long double centreX,
                                        long double centreY);
Coordinate getInterestingPoint(int mandleCounts[][yResolution], long double xStepDistance, long double yStepDistance, long double centreX, long double centreY,
                               int minXIndex,
                               int maxXIndex,
                               int minYIndex,
                               int maxYIndex);
std::vector<PixelIndex> getInterestingPixelIndexes(int mandleCounts[][yResolution],
                                                   int minXIndex,
                                                   int maxXIndex,
                                                   int minYIndex,
                                                   int maxYIndex);
int getPitchSum(std::vector<AubioNote> notes);
int sumAll(int start, int end);
#endif