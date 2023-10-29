#include <vector>
#include <cstdlib>
#include <cmath>
#include "math.h" // player.h must be in the current directory. or use relative or absolute path to it. e.g #include "include/player.h"

double getXPosition(
    int xSquare,
    double xStepDistance,
    double centreX)
{
    return centreX + (xSquare - 0.5 * xResolution) * xStepDistance;
};

double getYPosition(
    int ySquare,
    float yStepDistance,
    float centreY)
{
    return centreY + (ySquare - 0.5 * yResolution) * yStepDistance;
};

int Mandelbrot(double cr, double ci, int limit)
{
    int count = 0;
    double zr = 0.0;
    double zi = 0.0;
    double zr2 = 0.0;
    double zi2 = 0.0;
    while ((count < limit) && (zr2 + zi2 < 4.001))
    {
        double tzi = 2.0 * zr * zi + ci;
        zr = zr2 - zi2 + cr;
        zi = tzi;
        zr2 = zr * zr;
        zi2 = zi * zi;
        ++count;
    }
    return count;
}

std::vector<PixelIndex> getInterestingPixelIndexes(int mandleCounts[][yResolution],
                                                   int minXIndex,
                                                   int maxXIndex,
                                                   int minYIndex,
                                                   int maxYIndex)
{
    if (minXIndex < 1)
    {
        minXIndex = 1;
    }
    if (minYIndex < 1)
    {
        minYIndex = 1;
    }
    if (maxXIndex > xResolution - 1)
    {
        maxXIndex = xResolution - 1;
    }
    if (maxYIndex > yResolution - 1)
    {
        maxYIndex = yResolution - 1;
    }
    std::vector<PixelIndex> sufficientlyInterestingElements{};
    int interestingPointThreshold = 3;
    for (int x = minXIndex; x < maxXIndex; x++)
    {
        for (int y = minYIndex; y < maxYIndex; y++)
        {
            int localMandlenumber = mandleCounts[x][y];
            int differentNeighbours = 0;
            if (
                mandleCounts[x - 1][y - 1] != localMandlenumber)
            {
                differentNeighbours++;
            }
            if (mandleCounts[x - 1][y] != localMandlenumber)
            {
                differentNeighbours++;
            }
            if (
                mandleCounts[x - 1][y + 1] != localMandlenumber)
            {
                differentNeighbours++;
            }
            if (mandleCounts[x][y - 1] != localMandlenumber)
            {
                differentNeighbours++;
            }
            if (mandleCounts[x][y + 1] != localMandlenumber)
            {
                differentNeighbours++;
            }
            if (
                mandleCounts[x + 1][y - 1] != localMandlenumber)
            {
                differentNeighbours++;
            }
            if (mandleCounts[x + 1][y] != localMandlenumber)
            {
                differentNeighbours++;
            }
            if (
                mandleCounts[x + 1][y + 1] != localMandlenumber)
            {
                differentNeighbours++;
            }
            if (differentNeighbours > interestingPointThreshold)
            {
                PixelIndex interestingPoint;
                interestingPoint.xIndex = x;
                interestingPoint.yIndex = y;

                sufficientlyInterestingElements.push_back(interestingPoint);
            }
        }
    }
    return sufficientlyInterestingElements;
}

Coordinate chooseClosestInterestingPoint(std::vector<PixelIndex> interestingPoints,
                                         double xStepDistance,
                                         double yStepDistance,
                                         double centreX,
                                         double centreY,
                                         double targetX,
                                         double targetY)
{
    double closestDistanceSquared = 100000;
    Coordinate chosenInterestingPoint;
    Coordinate testInterestingPoint;
    for (unsigned int i = 0; i < interestingPoints.size(); i++)
    {
        testInterestingPoint.realPart = getXPosition(interestingPoints[i].xIndex, xStepDistance, centreX);
        testInterestingPoint.imaginaryPart = getYPosition(interestingPoints[i].yIndex, yStepDistance, centreY);
        double testDistanceSquared = pow((testInterestingPoint.realPart - targetX), 2) + pow((testInterestingPoint.imaginaryPart - targetY), 2);
        if (testDistanceSquared < closestDistanceSquared)
        {
            closestDistanceSquared = testDistanceSquared;
            chosenInterestingPoint.realPart = testInterestingPoint.realPart;
            chosenInterestingPoint.imaginaryPart = testInterestingPoint.imaginaryPart;
        }
    }

    return chosenInterestingPoint;
}

Coordinate chooseRandomInterestingPoint(std::vector<PixelIndex> interestingPoints,
                                        double xStepDistance,
                                        double yStepDistance,
                                        double centreX,
                                        double centreY)
{
    PixelIndex chosenPixIndex;
    chosenPixIndex.xIndex = 0;
    chosenPixIndex.yIndex = 0;
    if (interestingPoints.size() > 0)
    {
        int index = rand() % interestingPoints.size();
        chosenPixIndex.xIndex = interestingPoints[index].xIndex;
        chosenPixIndex.yIndex = interestingPoints[index].yIndex;
    }

    Coordinate nextInterestingPoint;
    nextInterestingPoint.realPart = getXPosition(chosenPixIndex.xIndex, xStepDistance, centreX);
    nextInterestingPoint.imaginaryPart = getYPosition(chosenPixIndex.yIndex, yStepDistance, centreY);
    return nextInterestingPoint;
}

Coordinate getInterestingPoint(
    int mandleCounts[][yResolution],
    double xStepDistance,
    double yStepDistance,
    double centreX,
    double centreY,
    int minXIndex,
    int maxXIndex,
    int minYIndex,
    int maxYIndex)
{
    std::vector<PixelIndex> maxBoundaryElements = getInterestingPixelIndexes(mandleCounts,
                                                                             minXIndex,
                                                                             maxXIndex,
                                                                             minYIndex,
                                                                             maxYIndex);
    return chooseRandomInterestingPoint(maxBoundaryElements,
                                        xStepDistance,
                                        yStepDistance,
                                        centreX,
                                        centreY);
}

int getPitchSum(std::vector<AubioNote> notes)
{
    int pitchSum = 0;
    for (unsigned int i = 0; i < notes.size(); i++)
    {
        pitchSum += notes[i].pitch;
    }
    return pitchSum;
}