#include <vector>
#include <cstdlib>
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

Coordinate GetInterestingPoint(int mandleCounts[][yResolution], double xStepDistance, double yStepDistance, double centreX, double centreY)
{
    std::vector<PixelIndex> maxBoundaryElements{};
    int interestingPointThreshold = 3;
    for (int x = 1; x < xResolution - 1; x++)
    {
        for (int y = 1; y < yResolution - 1; y++)
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

                maxBoundaryElements.push_back(interestingPoint);
            }
        }
    }
    PixelIndex chosenPixIndex;
    chosenPixIndex.xIndex = 0;
    chosenPixIndex.yIndex = 0;
    if (maxBoundaryElements.size() > 0)
    {
        int index = rand() % maxBoundaryElements.size();
        chosenPixIndex.xIndex = maxBoundaryElements[index].xIndex;
        chosenPixIndex.yIndex = maxBoundaryElements[index].yIndex;
    }

    Coordinate nextInterestingPoint;
    nextInterestingPoint.realPart = getXPosition(chosenPixIndex.xIndex, xStepDistance, centreX);
    nextInterestingPoint.imaginaryPart = getYPosition(chosenPixIndex.yIndex, yStepDistance, centreY);
    return nextInterestingPoint;
}
