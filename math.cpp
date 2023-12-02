#include "math.h" // player.h must be in the current directory. or use relative or absolute path to it. e.g #include "include/player.h"
#include <cmath>
#include <cstdlib>
#include <set>
#include <vector>

// #include <iostream>
long double getXPosition(int xSquare, long double cr_left,
                         long double xStepDistance) {
  return cr_left + xSquare * xStepDistance;
};

long double getYPosition(int ySquare, long double ci_top,
                         long double yStepDistance) {
  return ci_top - ySquare * yStepDistance;
};

int Mandelbrot(long double cr, long double ci, int limit) {
  int count = 0;
  long double zr = 0.0;
  long double zi = 0.0;
  long double zr2 = 0.0;
  long double zi2 = 0.0;
  while ((count < limit) && (zr2 + zi2 < 4.001)) {
    long double tzi = 2.0 * zr * zi + ci;
    zr = zr2 - zi2 + cr;
    zi = tzi;
    zr2 = zr * zr;
    zi2 = zi * zi;
    ++count;
  }
  return count;
}

std::vector<PixelIndex>
getInterestingPixelIndexes(int mandleCounts[xResolution][yResolution],
                           int minXIndex, int maxXIndex, int minYIndex,
                           int maxYIndex) {
  if (minXIndex < 1) {
    minXIndex = 1;
  }
  if (minYIndex < 1) {
    minYIndex = 1;
  }
  if (maxXIndex > xResolution - 1) {
    maxXIndex = xResolution - 1;
  }
  if (maxYIndex > yResolution - 1) {
    maxYIndex = yResolution - 1;
  }
  std::vector<PixelIndex> sufficientlyInterestingElements{};
  long unsigned int interestingPointThreshold = 3;
  for (int x = minXIndex; x < maxXIndex; x++) {
    for (int y = minYIndex; y < maxYIndex; y++) {
      std::set<int> uniqueNeighbours;
      for (int xDelta = -1; xDelta < 2; xDelta++) {
        for (int yDelta = -1; yDelta < 2; yDelta++) {
          uniqueNeighbours.insert(mandleCounts[x + xDelta][y + yDelta]);
        }
      }
      if (uniqueNeighbours.size() > interestingPointThreshold) {
        PixelIndex interestingPoint;
        interestingPoint.xIndex = x;
        interestingPoint.yIndex = y;
        // std::cout << "Found interesting point (" << x << ", " << y << "),
        // mandlenumber: " << mandleCounts[x][y] << ", unique neibours: " <<
        // uniqueNeighbours.size() << "\n"; for (int const &manldecountCompare :
        // uniqueNeighbours)
        // {
        //     std::cout << manldecountCompare << ' ';
        // }
        sufficientlyInterestingElements.push_back(interestingPoint);
      }
    }
  }
  return sufficientlyInterestingElements;
}

int sumAll(int start, int end) {
  int sum = 0;
  if (end < start) {
    return sum;
  }
  for (int i = start; i < end; i++) {
    sum += i;
  }
  return sum;
}

Coordinate chooseClosestInterestingPoint(
    std::vector<PixelIndex> interestingPoints, long double xStepDistance,
    long double yStepDistance, long double centreX, long double centreY,
    long double targetX, long double targetY, long double cr_left,
    long double ci_top) {
  long double closestDistanceSquared = 100000;
  Coordinate chosenInterestingPoint;
  Coordinate testInterestingPoint;
  for (unsigned int i = 0; i < interestingPoints.size(); i++) {
    testInterestingPoint.realPart =
        getXPosition(interestingPoints[i].xIndex, cr_left, xStepDistance);
    testInterestingPoint.imaginaryPart =
        getYPosition(interestingPoints[i].yIndex, ci_top, yStepDistance);
    long double testDistanceSquared =
        pow((testInterestingPoint.realPart - targetX), 2) +
        pow((testInterestingPoint.imaginaryPart - targetY), 2);
    if (testDistanceSquared < closestDistanceSquared) {
      closestDistanceSquared = testDistanceSquared;
      chosenInterestingPoint.realPart = testInterestingPoint.realPart;
      chosenInterestingPoint.imaginaryPart = testInterestingPoint.imaginaryPart;
    }
  }

  return chosenInterestingPoint;
}

Coordinate chooseRandomInterestingPoint(
    std::vector<PixelIndex> interestingPoints, long double xStepDistance,
    long double yStepDistance, long double centreX, long double centreY,
    long double cr_left, long double ci_top) {
  PixelIndex chosenPixIndex;
  chosenPixIndex.xIndex = 0;
  chosenPixIndex.yIndex = 0;
  if (interestingPoints.size() > 0) {
    int index = rand() % interestingPoints.size();
    chosenPixIndex.xIndex = interestingPoints[index].xIndex;
    chosenPixIndex.yIndex = interestingPoints[index].yIndex;
  }

  Coordinate nextInterestingPoint;
  nextInterestingPoint.realPart =
      getXPosition(chosenPixIndex.xIndex, cr_left, xStepDistance);
  nextInterestingPoint.imaginaryPart =
      getYPosition(chosenPixIndex.yIndex, ci_top, yStepDistance);
  return nextInterestingPoint;
}

Coordinate getInterestingPoint(int mandleCounts[][yResolution],
                               long double xStepDistance,
                               long double yStepDistance, long double centreX,
                               long double centreY, int minXIndex,
                               int maxXIndex, int minYIndex, int maxYIndex,
                               long double cr_left, long double ci_top) {
  std::vector<PixelIndex> maxBoundaryElements = getInterestingPixelIndexes(
      mandleCounts, minXIndex, maxXIndex, minYIndex, maxYIndex);
  return chooseRandomInterestingPoint(maxBoundaryElements, xStepDistance,
                                      yStepDistance, centreX, centreY, cr_left,
                                      ci_top);
}

int getPitchSum(std::vector<AubioNote> notes) {
  int pitchSum = 0;
  for (unsigned int i = 0; i < notes.size(); i++) {
    pitchSum += notes[i].pitch;
  }
  return pitchSum;
}