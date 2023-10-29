#ifndef MATH_H // To make sure you don't declare the function more than once by including the header multiple times.
#define MATH_H

#include "structs.h"
#include <vector>

// #include "stdafx.h"
// #include <SFML/Graphics.hpp>

int Mandelbrot(double cr, double ci, int limit);
Coordinate chooseClosestInterestingPoint(std::vector<PixelIndex> interestingPoints,
                                  double xStepDistance,
                                  double yStepDistance,
                                  double centreX,
                                  double centreY,
                                  double targetX,
                                  double targetY);
Coordinate chooseRandomInterestingPoint(std::vector<PixelIndex> interestingPoints,
                                  double xStepDistance,
                                  double yStepDistance,
                                  double centreX,
                                  double centreY );
Coordinate getInterestingPoint(int mandleCounts[][yResolution], double xStepDistance, double yStepDistance, double centreX, double centreY,
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

#endif