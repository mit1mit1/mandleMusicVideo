#ifndef MANDLESTRUCT_H // To make sure you don't declare the function more than
                       // once by including the header multiple times.
#define MANDLESTRUCT_H

#include <string>

struct PixelColor
{
  unsigned char red;
  unsigned char green;
  unsigned char blue;
  unsigned char alpha;
};

struct Ripple
{
  int xCenter;
  int yCenter;
  double speed;
  int thickness;
  int startFrame;
  int type;
  PixelColor addColor;
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

struct MidiNote
{
  int pitch;
  int volume;
  double startSeconds;
  double endSeconds;
  int trackNumber;
  bool isPercussion;
};

struct AubioNote
{
  float pitch;
  float startSeconds;
  float endSeconds;
};

struct MidiTrack
{
  std::string filename;
  bool isPercussion;
};

const int xResolution = 1280 * 3 / 2;
const int yResolution = 720 * 3 / 2;

const int squareSize = 1;

#endif
