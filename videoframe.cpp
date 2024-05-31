#include "videoframe.h"
#include "lodepng.h"
#include "math.h"
#include "structs.h"
#include <cmath>
#include <iostream>
#include <vector>

VideoFrame::VideoFrame(int _width, int _height)
    : width(_width), height(_height), buffer(4 * _width * _height, 255) {}

void VideoFrame::ScrollPixels(int xScrollSpeed, int yScrollSpeed,
                              PixelColor blankColor) {
  int xScrollMultiplier = 1;
  int yScrollMultiplier = 1;
  if (xScrollSpeed < 0) {
    xScrollMultiplier = xScrollMultiplier * -1;
  }
  if (yScrollSpeed < 0) {
    yScrollMultiplier = yScrollMultiplier * -1;
  }
  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      int destinationX =
          -(width - 1) * (-1 + xScrollMultiplier) / 2 + x * xScrollMultiplier;
      int destinationY =
          -(height - 1) * (-1 + yScrollMultiplier) / 2 + y * yScrollMultiplier;
      int sourceX = destinationX + xScrollSpeed;
      int sourceY = destinationY + yScrollSpeed;

      CopyPixel(sourceX, sourceY, destinationX, destinationY, blankColor);
    }
  }
};

// TODO: add copy buffer and use proper algo (
// https://stackoverflow.com/questions/299267/image-scaling-and-rotating-in-c-c
// )
void VideoFrame::CopySpunPixel(int destinationX, int destinationY, int originX,
                               int originY, double spinSpeedRadiansPerFrame,
                               double zoomMultiplierPerFrame,
                               PixelColor blankColor) {
  if (destinationX == originX) {
    return;
  }
  // std::cout << "Starting CopySpunPixel with " << destinationX << ", "
  //           << destinationY << ", " << originX << ", " << originY << ", "
  //           << " \n";
  double destinationRadians =
      std::atan2((destinationY - originY), (destinationX - originX));
  // std::cout << "Succeeded at getting atan " << destinationRadians << " \n";
  double sourceRadians = destinationRadians - spinSpeedRadiansPerFrame;

  // std::cout << "Succeeded at getting degrees \n";
  double destinationLength = std::sqrt(std::pow((destinationX - originX), 2) +
                                       std::pow((destinationY / originY), 2));
  // std::cout << "Succeeded at getting length " << destinationLength << " \n";
  double sourceLength = destinationLength / zoomMultiplierPerFrame;
  double sourceX = static_cast<int>(
      std::ceil(originX + std::cos(sourceRadians) * sourceLength));
  double sourceY = static_cast<int>(
      std::ceil(originY + std::sin(sourceRadians) * sourceLength));

  // std::cout << "Copying to (" << destinationX << ", " << destinationY << ")
  // from ("
  //           << sourceX << ", " << sourceY << ") "
  //           << " \n";
  CopyPixel(sourceX, sourceY, destinationX, destinationY, blankColor);
};

// TODO: add copy buffer and use proper algo (
// https://stackoverflow.com/questions/299267/image-scaling-and-rotating-in-c-c
// )
void VideoFrame::CopyDiscreteSpunPixel(int destinationX, int destinationY,
                                       int originX, int originY,
                                       double spinSpeedRadiansPerFrame,
                                       double zoomMultiplierPerFrame,
                                       PixelColor blankColor) {
  Coordinate zoomDiff =
      getDiscreteZoomDiff(destinationX, destinationY, originX, originY);
  CopyPixel(destinationX + zoomDiff.realPart,
            destinationY + zoomDiff.imaginaryPart, destinationX, destinationY,
            blankColor);
};

void VideoFrame::SpinZoomPixels(double spinSpeedRadiansPerFrame,
                                double zoomMultiplierPerFrame,
                                PixelColor blankColor) {
  int originX = width / 2;
  int originY = height / 2;
  for (int x = 0; x < width / 2; ++x) {
    for (int y = 0; y < height / 2; ++y) {
      CopyDiscreteSpunPixel(x, y, originX, originY, spinSpeedRadiansPerFrame,
                            zoomMultiplierPerFrame, blankColor);
      CopyDiscreteSpunPixel(width - 1 - x, height - 1 - y, originX, originY,
                            spinSpeedRadiansPerFrame, zoomMultiplierPerFrame,
                            blankColor);
    }
  }
  for (int x = 0; x < width / 2; ++x) {
    for (int y = 0; y < height / 2; ++y) {
      CopyDiscreteSpunPixel(x, height - 1 - y, originX, originY,
                            spinSpeedRadiansPerFrame, zoomMultiplierPerFrame,
                            blankColor);
      CopyDiscreteSpunPixel(width - 1 - x, y, originX, originY,
                            spinSpeedRadiansPerFrame, zoomMultiplierPerFrame,
                            blankColor);
    }
  }
};

void VideoFrame::CopyPixel(int sourceX, int sourceY, int destinationX,
                           int destinationY, PixelColor blankColor) {

  //   std::cout << "copying pixel (" << sourceX << ", " << sourceY << ") -> ("
  //             << destinationX << ", " << destinationY << ") \n";
  if (destinationX < 0 || destinationX >= width || destinationY < 0 ||
      destinationY >= height) {
    return;
  }
  int destinationIndex = 4 * (destinationY * width + destinationX);
  if (sourceX >= width || sourceX < 0 || sourceY >= height || sourceY < 0) {
    buffer[destinationIndex] = blankColor.red;
    buffer[destinationIndex + 1] = blankColor.green;
    buffer[destinationIndex + 2] = blankColor.blue;
    buffer[destinationIndex + 3] = blankColor.alpha;
    return;
  }
  int sourceIndex = 4 * (sourceY * width + sourceX);
  buffer[destinationIndex] = buffer[sourceIndex];
  buffer[destinationIndex + 1] = buffer[sourceIndex + 1];
  buffer[destinationIndex + 2] = buffer[sourceIndex + 2];
  buffer[destinationIndex + 3] = buffer[sourceIndex + 3];
};

void VideoFrame::BrightenAllPixels(float multiple) {
  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      BrightenPixel(x, y, multiple);
    }
  }
};

void VideoFrame::BrightenPixel(int x, int y, float multiple) {
  int index = 4 * (y * width + x);
  buffer[index] = (int)(buffer[index] * multiple);
  buffer[index + 1] = (int)(buffer[index + 1] * multiple);
  buffer[index + 2] = (int)(buffer[index + 2] * multiple);
  buffer[index + 3] = (int)(buffer[index + 3] * multiple);

  for (int k = 0; k < 4; ++k) {
    if (buffer[index + k] < 0) {
      buffer[index + k] = 0;
    }
    if (buffer[index + k] > 255) {
      buffer[index + k] = 255;
    }
  }
};

void VideoFrame::CombinePixel(int x, int y, float multiple,
                              PixelColor targetColor, int maxSaturation) {
  int index = 4 * (y * width + x);
  buffer[index] = (int)(buffer[index] * multiple);
  if (buffer[index] < maxSaturation) {
    buffer[index] += (int)((1 - multiple) * targetColor.red);
  };

  buffer[index + 1] = (int)(buffer[index + 1] * multiple);
  if (buffer[index + 1] < maxSaturation) {
    buffer[index + 1] += (int)((1 - multiple) * targetColor.green);
  };

  buffer[index + 2] = (int)(buffer[index + 2] * multiple);
  if (buffer[index + 2] < maxSaturation) {
    buffer[index + 2] += (int)((1 - multiple) * targetColor.blue);
  }

  buffer[index + 3] = (int)(buffer[index + 3] * multiple);
  if (buffer[index + 3] < maxSaturation) {
    buffer[index + 3] += (int)((1 - multiple) * targetColor.alpha);
  }

  for (int k = 0; k < 4; ++k) {
    if (buffer[index + k] < 0) {
      buffer[index + k] = 0;
    }
    if (buffer[index + k] > 255) {
      buffer[index + k] = 255;
    }
  }
};

void VideoFrame::AddPixel(int x, int y, PixelColor color) {
  int index = 4 * (y * width + x);
  buffer[index] = buffer[index] + color.red;
  buffer[index + 1] = buffer[index + 1] + color.green;
  buffer[index + 2] = buffer[index + 2] + color.blue;
  buffer[index + 3] = buffer[index + 3] + color.alpha;
  for (int k = 0; k < 4; ++k) {
    if (buffer[index + k] < 0) {
      buffer[index + k] = 0;
    }
    if (buffer[index + k] > 255) {
      buffer[index + k] = 255;
    }
  }
};

void VideoFrame::SetAllPixels(PixelColor color) {
  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      SetPixel(x, y, color);
    }
  }
};

void VideoFrame::SetPixel(int x, int y, PixelColor color) {
  int index = 4 * (y * width + x);
  buffer[index] = color.red;
  buffer[index + 1] = color.green;
  buffer[index + 2] = color.blue;
  buffer[index + 3] = color.alpha;

  for (int k = 0; k < 4; ++k) {
    if (buffer[index + k] < 0) {
      buffer[index + k] = 0;
    }
    if (buffer[index + k] > 255) {
      buffer[index + k] = 255;
    }
  }
};

PixelColor VideoFrame::GetPixel(int x, int y) {
  int index = 4 * (y * width + x);
  PixelColor color;
  color.red = buffer[index];
  color.green = buffer[index + 1];
  color.blue = buffer[index + 2];
  color.alpha = buffer[index + 3];
  return color;
};

int VideoFrame::SavePng(const char *outFileName) {
  unsigned error = lodepng::encode(outFileName, buffer, width, height);
  if (error) {
    fprintf(stderr, "ERROR: lodepng::encode returned %u\n", error);
    return 1;
  }
  return 0;
};