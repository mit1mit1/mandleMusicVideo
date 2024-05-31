#ifndef VIDEOFRAME_H // To make sure you don't declare the function more than
                     // once by
                     // including the header multiple times.
#define VIDEOFRAME_H

#include "lodepng.h"
#include "structs.h"
#include <vector>

class VideoFrame {
private:
  int width;
  int height;
  std::vector<unsigned char> buffer;

public:
  VideoFrame(int _width, int _height);

  void ScrollPixels(int xScrollSpeed, int yScrollSpeed, PixelColor blankColor);

  void SpinZoomPixels(double spinSpeedRadiansPerFrame,
                      double zoomMultiplierPerFrame, PixelColor blankColor);

  void CopySpunPixel(int sourceX, int sourceY, int originX, int originY,
                     double spinSpeedRadiansPerFrame, double zoomMultiplierPerFrame,
                     PixelColor blankColor);

  void CopyPixel(int sourceX, int sourceY, int destinationX, int destinationY,
                 PixelColor blankColor);

  void BrightenAllPixels(float multiple);

  void BrightenPixel(int x, int y, float multiple);

  void CombinePixel(int x, int y, float multiple, PixelColor targetColor,
                    int maxSaturation);

  void AddPixel(int x, int y, PixelColor color);

  void SetAllPixels(PixelColor color);

  void SetPixel(int x, int y, PixelColor color);

  PixelColor GetPixel(int x, int y);

  int SavePng(const char *outFileName);
};
#endif