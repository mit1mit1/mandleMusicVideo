#ifndef MATH_H // To make sure you don't declare the function more than once by
               // including the header multiple times.
#define MATH_H

#include "structs.h"
#include <vector>

// #include "stdafx.h"
// #include <SFML/Graphics.hpp>

int Mandelbrot(long double cr, long double ci, int limit);
Coordinate chooseClosestInterestingPoint(
    std::vector<PixelIndex> interestingPoints, long double xStepDistance,
    long double yStepDistance, long double centreX, long double centreY,
    long double targetX, long double targetY,

    long double cr_left, long double ci_top);
Coordinate chooseRandomInterestingPoint(
    std::vector<PixelIndex> interestingPoints, long double xStepDistance,
    long double yStepDistance, long double centreX, long double centreY,
    long double cr_left, long double ci_top);
Coordinate getInterestingPoint(int mandleCounts[][yResolution],
                               long double xStepDistance,
                               long double yStepDistance, long double centreX,
                               long double centreY, int minXIndex,
                               int maxXIndex, int minYIndex, int maxYIndex,
                               long double cr_left, long double ci_top);
std::vector<PixelIndex>
getInterestingPixelIndexes(int mandleCounts[xResolution][yResolution],
                           int minXIndex, int maxXIndex, int minYIndex,
                           int maxYIndex);
long double getXPosition(int xSquare, long double cr_left,
                         long double xStepDistance);
long double getYPosition(int ySquare, long double ci_top,
                         long double yStepDistance);
int getPitchSum(std::vector<AubioNote> notes);
int getMinPitch(std::vector<AubioNote> notes);
int getMaxPitch(std::vector<AubioNote> notes);
int getMinMinPitch(std::vector<std::vector<AubioNote>> notesVec);
int getMaxMaxPitch(std::vector<std::vector<AubioNote>> notesVec);
int sumAll(int start, int end);
#endif