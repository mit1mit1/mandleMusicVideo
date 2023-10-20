#ifndef MANDLESTRUCT_H    // To make sure you don't declare the function more than once by including the header multiple times.
#define MANDLESTRUCT_H

struct PixelColor
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
};

struct Coordinate
{
    double realPart;
    double imaginaryPart;
};

struct PixelIndex
{
    int xIndex;
    int yIndex;
};

struct AubioNote
{
    float pitch;
    float startSeconds;
    float endSeconds;
};

const int xResolution = 1280;
const int yResolution = 720;

#endif
