#include "art.h" // art.h must be in the current directory. or use relative or absolute path to it. e.g #include "math/player.h"
#include "structs.h"
#include "videoframe.h"
#include <vector>

Ripple getNoteRippleSidescrolling(int minX, int maxX, int minY, int maxY,
                                  AubioNote currentNote,
                                  std::vector<int> minPitches,
                                  std::vector<int> pitchRanges,
                                  int framespersecond, int intrumentNumber) {
  Ripple newRipple;
  // TODO: Since we are scrolling on x axis now maybe don't keep
  // instruments in x quadrants - use color instead?
  newRipple.xCenter = (int)(xResolution / 3) +
                      (int)(((currentNote.pitch - minPitches[intrumentNumber]) /
                             pitchRanges[intrumentNumber]) *
                            (intrumentNumber + 3) * 7 * xResolution / 31) %
                          (int)(xResolution / (2));
  newRipple.yCenter = 9 * yResolution / 10 -
                      (int)(((currentNote.pitch - minPitches[intrumentNumber]) /
                             pitchRanges[intrumentNumber]) *
                            (8 * yResolution / 10));
  int speedBonus =
      (int)(0.3 / (currentNote.startSeconds - currentNote.endSeconds));
  if (speedBonus > 6) {
    speedBonus = 6;
  }
  newRipple.speed = 3 + speedBonus;
  newRipple.thickness = 90;
  newRipple.startFrame = currentNote.startSeconds * framespersecond;
  PixelColor perimColor1;
  perimColor1.red =
      (int)((currentNote.pitch *
             (((intrumentNumber + 1) + 11 + (intrumentNumber + 1) * 11) % 3 +
              1)) *
            0.25);
  perimColor1.green =
      (int)((currentNote.pitch *
             (((intrumentNumber + 1) + 11 + (intrumentNumber + 1) * 11) % 5 +
              1)) *
            0.25);
  perimColor1.blue =
      (int)((currentNote.pitch *
             (((intrumentNumber + 1) + 11 + (intrumentNumber + 1) * 11) % 7 +
              1)) *
            0.25);
  perimColor1.alpha = 0;
  newRipple.addColor = perimColor1;
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