#include "art.h" // art.h must be in the current directory. or use relative or absolute path to it. e.g #include "math/player.h"
#include "structs.h"
#include "videoframe.h"
#include <cmath>
#include <iostream>
#include <vector>

Ripple getNoteRippleCircleOfScales(int width, int height, double notePitch,
                                   double noteStartSeconds,
                                   double noteEndSeconds, int framespersecond,
                                   int instrumentNumber) {
  Ripple newRipple;

  double maxRadius = std::min(width / 2, height / 2) * 9 / 10;

  double angle = notePitch * M_PI / 6;
  // std::cout << "angle of ripple" << angle << "\n";

  // Assume highest pitch ever is 120
  double radius = maxRadius * (0.02 + 0.98 * notePitch / 120);

  newRipple.xCenter = (int)(width / 2 + radius * std::cos(angle));
  newRipple.yCenter = (int)(height / 2 + radius * std::sin(angle));
  int speedBonus = (int)(0.2 / (noteStartSeconds - noteEndSeconds));
  if (speedBonus > 6) {
    speedBonus = 6;
  }
  newRipple.speed = 1 + speedBonus;
  newRipple.thickness = 60;
  newRipple.startFrame = noteStartSeconds * framespersecond;

  newRipple.addColor = getRippleColor(notePitch, instrumentNumber);
  newRipple.type = instrumentNumber % 5;
  return newRipple;
}

// TODO: Array of nice colors
PixelColor getRippleColor(double notePitch, int instrumentNumber) {
  PixelColor rippleColor;
  rippleColor.red =
      (int)((notePitch *
             (((instrumentNumber + 1) + 11 + (instrumentNumber + 1) * 11) % 3 +
              1)) *
            0.25);
  rippleColor.green =
      (int)((notePitch *
             (((instrumentNumber + 1) + 11 + (instrumentNumber + 1) * 11) % 5 +
              1)) *
            0.25);
  rippleColor.blue =
      (int)((notePitch *
             (((instrumentNumber + 1) + 11 + (instrumentNumber + 1) * 11) % 7 +
              1)) *
            0.25);
  rippleColor.alpha = 255;
  return rippleColor;
}

Ripple getNoteRippleSidescrolling(int minX, int maxX, int minY, int maxY,
                                  AubioNote currentNote,
                                  std::vector<int> minPitches,
                                  std::vector<int> pitchRanges,
                                  int framespersecond, int instrumentNumber) {
  Ripple newRipple;
  // TODO: Since we are scrolling on x axis now maybe don't keep
  // instruments in x quadrants - use color instead?
  newRipple.xCenter =
      (int)(xResolution / 3) +
      (int)(((currentNote.pitch - minPitches[instrumentNumber]) /
             pitchRanges[instrumentNumber]) *
            (instrumentNumber + 3) * 7 * xResolution / 31) %
          (int)(xResolution / (2));
  newRipple.yCenter =
      9 * yResolution / 10 -
      (int)(((currentNote.pitch - minPitches[instrumentNumber]) /
             pitchRanges[instrumentNumber]) *
            (8 * yResolution / 10));
  int speedBonus =
      (int)(0.2 / (currentNote.startSeconds - currentNote.endSeconds));
  if (speedBonus > 6) {
    speedBonus = 6;
  }
  newRipple.speed = 1 + speedBonus;
  newRipple.thickness = 60;
  newRipple.startFrame = currentNote.startSeconds * framespersecond;
  newRipple.addColor = getRippleColor(currentNote.pitch, instrumentNumber);
  newRipple.type = instrumentNumber % 5;
  return newRipple;
}

PixelColor getNextBackgroundColor(double timestamp, double lastOnsetTimestamp,
                                  double onsetColorChangeLength,
                                  int onsetsPassed,
                                  PixelColor currentBackgroundColor,
                                  int backgroundColorMaxSaturation) {
  PixelColor nextBackgroundColor;
  // TODO: Use background color
  if (timestamp < lastOnsetTimestamp + onsetColorChangeLength) {
    if (onsetsPassed % 2 == 0 &&
        currentBackgroundColor.red < backgroundColorMaxSaturation - 1) {
      nextBackgroundColor.red = currentBackgroundColor.red + 1;
    } else {
      nextBackgroundColor.red = currentBackgroundColor.red - 1;
    }
    if (onsetsPassed % 2 == 0 &&
        currentBackgroundColor.blue < backgroundColorMaxSaturation - 3) {
      nextBackgroundColor.blue = currentBackgroundColor.blue + 3;
    } else {
      nextBackgroundColor.blue = currentBackgroundColor.blue - 3;
    }
    if (onsetsPassed % 2 == 0 &&
        currentBackgroundColor.green < backgroundColorMaxSaturation - 2) {
      nextBackgroundColor.green = currentBackgroundColor.green + 2;
    } else {
      nextBackgroundColor.green = currentBackgroundColor.green - 2;
    }
  }

  return nextBackgroundColor;
}

void colorRipples(int width, int height, std::vector<Ripple> ripples,
                  int framenumber, int scrollSpeedX, int scrollSpeedY,
                  VideoFrame currentFrame) {
  for (Ripple ripple : ripples) {
    int framesSinceRippleStart = framenumber - ripple.startFrame;
    int radius = framesSinceRippleStart * ripple.speed + 12;
    int thickness =
        (ripple.thickness + (radius / 2)) * ripple.thickness + (radius / 2);
    int scrolledXCenter =
        ripple.xCenter - framesSinceRippleStart * scrollSpeedX;
    int scrolledYCenter =
        ripple.yCenter - framesSinceRippleStart * scrollSpeedY;
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        const int distFromCentreSquared =
            std::pow((x - scrolledXCenter), ripple.type + 1) *
                std::cos(ripple.type * 3 + 3 / 4) +
            std::pow((y - scrolledYCenter), ripple.type + 1) *
                std::sin(ripple.type * 3 + 3 / 4);
        if (distFromCentreSquared > radius * radius - thickness &&
            distFromCentreSquared < radius * radius) {
          currentFrame.AddPixel(x, y, ripple.addColor);
        }
      }
    }
  }
}