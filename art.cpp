#include "art.h" // art.h must be in the current directory. or use relative or absolute path to it. e.g #include "math/player.h"
#include "structs.h"
#include "videoframe.h"
#include <cmath>
#include <iostream>
#include <vector>

Ripple getNoteRippleCircleOfScales(int width, int height, AubioNote currentNote,
                                   int framespersecond, int instrumentNumber) {
  Ripple newRipple;

  double maxRadius = std::min(width / 2, height / 2) * 9 / 10;

  double angle = currentNote.pitch * M_PI / 6;
  std::cout << "angle of ripple" << angle << "\n";

  // Assume highest pitch ever is 100?
  double radius = maxRadius * (0.3 + 0.7 * currentNote.pitch / 100);

  newRipple.xCenter = (int)(width / 2 + radius * std::cos(angle));
  newRipple.yCenter = (int)(height / 2 + radius * std::sin(angle));
  int speedBonus =
      (int)(0.3 / (currentNote.startSeconds - currentNote.endSeconds));
  if (speedBonus > 6) {
    speedBonus = 6;
  }
  newRipple.speed = 3 + speedBonus;
  newRipple.thickness = 90;
  newRipple.startFrame = currentNote.startSeconds * framespersecond;

  newRipple.addColor = getRippleColor(currentNote, instrumentNumber);
  return newRipple;
}

// TODO: Array of nice colors
PixelColor getRippleColor(AubioNote currentNote, int instrumentNumber) {
  PixelColor rippleColor;
  rippleColor.red =
      (int)((currentNote.pitch *
             (((instrumentNumber + 1) + 11 + (instrumentNumber + 1) * 11) % 3 +
              1)) *
            0.25);
  rippleColor.green =
      (int)((currentNote.pitch *
             (((instrumentNumber + 1) + 11 + (instrumentNumber + 1) * 11) % 5 +
              1)) *
            0.25);
  rippleColor.blue =
      (int)((currentNote.pitch *
             (((instrumentNumber + 1) + 11 + (instrumentNumber + 1) * 11) % 7 +
              1)) *
            0.25);
  rippleColor.alpha = 0;
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
      (int)(0.3 / (currentNote.startSeconds - currentNote.endSeconds));
  if (speedBonus > 6) {
    speedBonus = 6;
  }
  newRipple.speed = 3 + speedBonus;
  newRipple.thickness = 90;
  newRipple.startFrame = currentNote.startSeconds * framespersecond;
  newRipple.addColor = getRippleColor(currentNote, instrumentNumber);

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
    if (onsetsPassed % 2 == 0 &&
        currentBackgroundColor.alpha < backgroundColorMaxSaturation - 3) {
      nextBackgroundColor.alpha = currentBackgroundColor.alpha + 3;
    } else {
      nextBackgroundColor.alpha = currentBackgroundColor.alpha - 3;
    }
  }

  return nextBackgroundColor;
}

void colorRipples(int width, int height, std::vector<Ripple> ripples,
                  int framenumber, int scrollSpeedX, int scrollSpeedY,
                  VideoFrame currentFrame) {
  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      for (Ripple ripple : ripples) {

        int framesSinceRippleStart = framenumber - ripple.startFrame;
        int radius = framesSinceRippleStart * ripple.speed + 5;
        int thickness =
            (ripple.thickness + (radius / 2)) * ripple.thickness + (radius / 2);
        int scrolledXCenter =
            ripple.xCenter - framesSinceRippleStart * scrollSpeedX;
        int scrolledYCenter =
            ripple.yCenter - framesSinceRippleStart * scrollSpeedY;
        const int distFromCentreSquared =
            (x - scrolledXCenter) * (x - scrolledXCenter) +
            (y - scrolledYCenter) * (y - scrolledYCenter);
        if (distFromCentreSquared > radius * radius - thickness &&
            distFromCentreSquared < radius * radius) {

          currentFrame.AddPixel(x, y, ripple.addColor);
        }
      }
    }
  }
}