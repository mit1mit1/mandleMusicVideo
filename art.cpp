#include "art.h" // art.h must be in the current directory. or use relative or absolute path to it. e.g #include "math/player.h"
#include "structs.h"
#include "videoframe.h"
#include <cmath>
#include <iostream>
#include <vector>

Ripple getNoteRippleCircleOfScales(int width, int height, double notePitch,
                                   double noteStartSeconds,
                                   double noteEndSeconds, int framespersecond,
                                   int instrumentNumber, bool isPercussion)
{
  Ripple newRipple;
  const double frameSpeedMultiplier = 0.5;
  if (isPercussion)
  {
    // Offset with tuned notes
    double angle = (notePitch * 7) * M_PI / 12 + M_PI / 24;
    // std::cout << "angle of ripple" << angle << "\n";

    // Keep near the center
    double radius = std::min(width / 2, height / 2) * 1 / 10;
    newRipple.xCenter = (int)(width / 2 + radius * std::cos(angle));
    newRipple.yCenter = (int)(height / 2 + radius * std::sin(angle));
    newRipple.speed = 10 * frameSpeedMultiplier;
  }
  else
  {
    double angle = notePitch * M_PI / 6;
    // std::cout << "angle of ripple" << angle << "\n";

    // Assume highest pitch ever is 120
    double maxRadius = std::min(width / 2, height / 2) * 9 / 10;
    double radius = maxRadius * (0.02 + 0.98 * notePitch / 120);
    newRipple.xCenter = (int)(width / 2 + radius * std::cos(angle));
    newRipple.yCenter = (int)(height / 2 + radius * std::sin(angle));
    int speedBonus = (int)(0.2 / (noteStartSeconds - noteEndSeconds));
    if (speedBonus > 3)
    {
      speedBonus = 3;
    }
    newRipple.speed = (1 + speedBonus) * frameSpeedMultiplier;
  }

  newRipple.thickness = 60;
  newRipple.startFrame = noteStartSeconds * framespersecond;

  // TODO switch this based on number of input files
  newRipple.addColor = getRippleColorPitch(notePitch, instrumentNumber);
  // newRipple.addColor = getRippleColorPitch(notePitch, instrumentNumber);
  // newRipple.addColor = getRippleColorPitchDependent(notePitch,
  // instrumentNumber);
  newRipple.type = instrumentNumber % 5;
  return newRipple;
}

PixelColor getRippleColorPitch(double notePitch, int instrumentNumber)
{

  std::vector<PixelColor> mrMarsColorWheel = {
      // F# Red
      PixelColor{.red = 255, .green = 0, .blue = 0, .alpha = 255},
      // G Chrysolite
      PixelColor{.red = 0, .green = 255, .blue = 180, .alpha = 255},
      // Ab Magenta
      PixelColor{.red = 255, .green = 19, .blue = 255, .alpha = 255},
      // A Chatruse
      PixelColor{.red = 163, .green = 252, .blue = 0, .alpha = 255},
      // Bb Corn Flower
      PixelColor{.red = 147, .green = 153, .blue = 255, .alpha = 255},
      // B Tumeric
      PixelColor{.red = 255, .green = 191, .blue = 0, .alpha = 255},
      // C Cyan
      PixelColor{.red = 0, .green = 255, .blue = 255, .alpha = 255},
      // Db Pink
      PixelColor{.red = 255, .green = 147, .blue = 225, .alpha = 255},
      // D Bright Green
      PixelColor{.red = 0, .green = 250, .blue = 0, .alpha = 255},
      // Eb Violet
      PixelColor{.red = 187, .green = 58, .blue = 255, .alpha = 255},
      // E Yellow
      PixelColor{.red = 255, .green = 253, .blue = 0, .alpha = 255},
      // F Azure
      PixelColor{.red = 0, .green = 180, .blue = 255, .alpha = 255},
  };

  std::vector<PixelColor> autumnPallette = {
      // Macadamia
      PixelColor{.red = 238, .green = 224, .blue = 200, .alpha = 255},
      // Dark Ginger
      PixelColor{.red = 172, .green = 92, .blue = 56, .alpha = 255},
      // Amber
      PixelColor{.red = 214, .green = 120, .blue = 52, .alpha = 255},
      // Laguna 2
      PixelColor{.red = 0, .green = 100, .blue = 118, .alpha = 255},
      // Brick red
      PixelColor{.red = 211, .green = 58, .blue = 52, .alpha = 255},
      // Olive
      PixelColor{.red = 147, .green = 152, .blue = 94, .alpha = 255},
      // Camel
      PixelColor{.red = 180, .green = 124, .blue = 65, .alpha = 255},
      // Laguna 1
      PixelColor{.red = 29, .green = 90, .blue = 107, .alpha = 255},
      // Soft Olive
      PixelColor{.red = 125, .green = 123, .blue = 79, .alpha = 255},
      // Mustard
      PixelColor{.red = 230, .green = 176, .blue = 81, .alpha = 255},
      // Chocolate
      PixelColor{.red = 50, .green = 39, .blue = 19, .alpha = 255},
      // Spiced Coral
      PixelColor{.red = 229, .green = 110, .blue = 104, .alpha = 255},
  };

  std::vector<PixelColor> bladeRunnerPallette = {
      // Orange
      PixelColor{.red = 135, .green = 12, .blue = 2, .alpha = 255},
      // Blue
      PixelColor{.red = 1, .green = 39, .blue = 54, .alpha = 255},
      // Orange
      PixelColor{.red = 157, .green = 28, .blue = 2, .alpha = 255},
      // Blue
      PixelColor{.red = 0, .green = 66, .blue = 78, .alpha = 255},
      // Orange
      PixelColor{.red = 186, .green = 60, .blue = 2, .alpha = 255},
      // Blue
      PixelColor{.red = 0, .green = 95, .blue = 109, .alpha = 255},
      // Orange
      PixelColor{.red = 214, .green = 100, .blue = 2, .alpha = 255},
      // Blue
      PixelColor{.red = 0, .green = 1, .blue = 115, .alpha = 255},
      // Orange
      PixelColor{.red = 188, .green = 64, .blue = 5, .alpha = 255},
      // Blue
      PixelColor{.red = 0, .green = 44, .blue = 59, .alpha = 255},
      // Orange
      PixelColor{.red = 50, .green = 39, .blue = 19, .alpha = 255},
      // Blue
      PixelColor{.red = 2, .green = 60, .blue = 90, .alpha = 255},
  };
  // std::vector<PixelColor> mrMarsColorWheelPastelled = {
  //     // F# Red
  //     PixelColor{.red = 101, .green = 15, .blue = 0},
  //     // G Chrysolity
  //     PixelColor{.red = 0, .green = 98, .blue = 73},
  //     // Ab Magenta
  //     PixelColor{.red = 101, .green = 25, .blue = 100},
  //     // A Chatruse
  //     PixelColor{.red = 72, .green = 98, .blue = 0},
  //     // Bb Corn Flower
  //     PixelColor{.red = 58, .green = 60, .blue = 100},
  //     // B Tumeric
  //     PixelColor{.red = 100, .green = 76, .blue = 19},
  //     // C Cyan
  //     PixelColor{.red = 0, .green = 99, .blue = 100},
  //     // Db Pink
  //     PixelColor{.red = 101, .green = 60, .blue = 87},
  //     // D Bright Green
  //     PixelColor{.red = 0, .green = 97, .blue = 0},
  //     // Eb Violet
  //     PixelColor{.red = 73, .green = 22, .blue = 100},
  //     // E Yellow
  //     PixelColor{.red = 100, .green = 99, .blue = 0},
  //     // F Azure
  //     PixelColor{.red = 0, .green = 73, .blue = 100},
  // };
  // std::vector<PixelColor> zeldaColors = {
  //     PixelColor{.red = 90, .green = 151, .blue = 255},
  //     PixelColor{.red = 0, .green = 243, .blue = 54},
  //     PixelColor{.red = 166, .green = 87, .blue = 255},
  //     PixelColor{.red = 0, .green = 56, .blue = 152},
  //     PixelColor{.red = 72, .green = 0, .blue = 155},
  //     PixelColor{.red = 72, .green = 255, .blue = 113},
  //     PixelColor{.red = 51, .green = 20, .blue = 86},
  //     PixelColor{.red = 0, .green = 173, .blue = 38},
  //     PixelColor{.red = 6, .green = 92, .blue = 239},
  //     PixelColor{.red = 115, .green = 5, .blue = 239},
  //     PixelColor{.red = 21, .green = 44, .blue = 85},
  //     PixelColor{.red = 16, .green = 96, .blue = 34},
  // };
  // Dark mode
  // const double frameSpeedDarkener = 0.6;
  // Light mode

  // Defined blade runner pallete in alternating colors hues - will make each instrument one hue
  const int palleteIndex = (2 * ((int)(notePitch) % 6) + instrumentNumber % 2) % 12;
  PixelColor rippleColor = bladeRunnerPallette[(2 * ((int)(notePitch) % 6) + instrumentNumber % 2) % 12];
  std::cout << "ripple color from pallete index " << +palleteIndex << ": (" << +rippleColor.red << ", " << +rippleColor.green << ", " << +rippleColor.blue << ") " << "\n";

  // PixelColor rippleColor = mrMarsColorWheel[(int)(notePitch) % 12];
  // Dark add mode
  // rippleColor.red = (unsigned char)(int)(rippleColor.red * frameSpeedDarkener * (0.08 + 0.05 * notePitch / 72));
  // rippleColor.green = (unsigned char)(int)(rippleColor.green * frameSpeedDarkener * (0.08 + 0.05 * notePitch / 72));
  // rippleColor.blue = (unsigned char)(int)(rippleColor.blue * frameSpeedDarkener * (0.08 + 0.05 * notePitch / 72));

  // Light combine mode
  rippleColor.red = (unsigned char)(int)(rippleColor.red);
  rippleColor.green = (unsigned char)(int)(rippleColor.green);
  rippleColor.blue = (unsigned char)(int)(rippleColor.blue);

  std::cout << "ripple color after calc: (" << +rippleColor.red << ", " << +rippleColor.green << ", " << +rippleColor.blue << ")" << "\n";
  return rippleColor;
}

PixelColor getRippleColorInstrument(double notePitch, int instrumentNumber)
{

  std::vector<PixelColor> mrMarsColorWheel = {
      // F# Red
      PixelColor{.red = 255, .green = 0, .blue = 0},
      // G Chrysolite
      PixelColor{.red = 0, .green = 255, .blue = 180},
      // Ab Magenta
      PixelColor{.red = 255, .green = 19, .blue = 255},
      // A Chatruse
      PixelColor{.red = 163, .green = 252, .blue = 0},
      // Bb Corn Flower
      PixelColor{.red = 147, .green = 153, .blue = 255},
      // B Tumeric
      PixelColor{.red = 255, .green = 191, .blue = 0},
      // C Cyan
      PixelColor{.red = 0, .green = 255, .blue = 255},
      // Db Pink
      PixelColor{.red = 255, .green = 147, .blue = 225},
      // D Bright Green
      PixelColor{.red = 0, .green = 250, .blue = 0},
      // Eb Violet
      PixelColor{.red = 187, .green = 58, .blue = 255},
      // E Yellow
      PixelColor{.red = 255, .green = 253, .blue = 0},
      // F Azure
      PixelColor{.red = 0, .green = 180, .blue = 255},
  };
  const double frameSpeedDarkener = 0.6;
  PixelColor rippleColor = mrMarsColorWheel[(int)(instrumentNumber) % 12];
  rippleColor.red = (int)(rippleColor.red * frameSpeedDarkener * (0.08 + 0.05 * notePitch / 72));
  rippleColor.green = (int)(rippleColor.green * frameSpeedDarkener * (0.08 + 0.05 * notePitch / 72));
  rippleColor.blue = (int)(rippleColor.blue * frameSpeedDarkener * (0.08 + 0.05 * notePitch / 72));
  return rippleColor;
}

// // TODO: Array of nice colors
// PixelColor getRippleColorPitchDependent(double notePitch, int
// instrumentNumber) {
//   PixelColor rippleColor;
//   rippleColor.red =
//       (int)((notePitch *
//              (((instrumentNumber + 1) + 11 + (instrumentNumber + 1) * 11) % 3
//              +
//               1)) *
//             0.25);
//   rippleColor.green =
//       (int)((notePitch *
//              (((instrumentNumber + 1) + 11 + (instrumentNumber + 1) * 11) % 5
//              +
//               1)) *
//             0.25);
//   rippleColor.blue =
//       (int)((notePitch *
//              (((instrumentNumber + 1) + 11 + (instrumentNumber + 1) * 11) % 7
//              +
//               1)) *
//             0.25);
//   rippleColor.alpha = 255;
//   return rippleColor;
// }

Ripple getNoteRippleSidescrolling(int minX, int maxX, int minY, int maxY,
                                  AubioNote currentNote,
                                  std::vector<int> minPitches,
                                  std::vector<int> pitchRanges,
                                  int framespersecond, int instrumentNumber)
{
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
  if (speedBonus > 6)
  {
    speedBonus = 6;
  }
  newRipple.speed = 1 + speedBonus;
  newRipple.thickness = 60;
  newRipple.startFrame = currentNote.startSeconds * framespersecond;
  newRipple.addColor = getRippleColorInstrument(currentNote.pitch, instrumentNumber);
  newRipple.type = instrumentNumber % 5;
  return newRipple;
}

PixelColor getNextBackgroundColor(double timestamp, double lastOnsetTimestamp,
                                  double onsetColorChangeLength,
                                  int onsetsPassed,
                                  PixelColor currentBackgroundColor,
                                  int backgroundColorMaxSaturation)
{
  PixelColor nextBackgroundColor;
  // TODO: Use background color
  if (timestamp < lastOnsetTimestamp + onsetColorChangeLength)
  {
    if (onsetsPassed % 2 == 0 &&
        currentBackgroundColor.red < backgroundColorMaxSaturation - 1)
    {
      nextBackgroundColor.red = currentBackgroundColor.red + 1;
    }
    else
    {
      nextBackgroundColor.red = currentBackgroundColor.red - 1;
    }
    if (onsetsPassed % 2 == 0 &&
        currentBackgroundColor.blue < backgroundColorMaxSaturation - 3)
    {
      nextBackgroundColor.blue = currentBackgroundColor.blue + 3;
    }
    else
    {
      nextBackgroundColor.blue = currentBackgroundColor.blue - 3;
    }
    if (onsetsPassed % 2 == 0 &&
        currentBackgroundColor.green < backgroundColorMaxSaturation - 2)
    {
      nextBackgroundColor.green = currentBackgroundColor.green + 2;
    }
    else
    {
      nextBackgroundColor.green = currentBackgroundColor.green - 2;
    }
  }

  return nextBackgroundColor;
}

void colorRipples(int width, int height, std::vector<Ripple> ripples,
                  int framenumber, int scrollSpeedX, int scrollSpeedY,
                  VideoFrame currentFrame)
{
  for (Ripple ripple : ripples)
  {
    int framesSinceRippleStart = framenumber - ripple.startFrame;
    int radius = framesSinceRippleStart * ripple.speed + 12;
    int thickness =
        (ripple.thickness + (radius / 2)) * ripple.thickness + (radius / 2);
    int scrolledXCenter =
        ripple.xCenter - framesSinceRippleStart * scrollSpeedX;
    int scrolledYCenter =
        ripple.yCenter - framesSinceRippleStart * scrollSpeedY;
    for (int x = 0; x < width; ++x)
    {
      for (int y = 0; y < height; ++y)
      {
        // const int distFromCentreSquaredWonky =
        //     std::pow((x - scrolledXCenter), ripple.type + 1) *
        //         std::cos(ripple.type * 3 + 3 / 4) +
        //     std::pow((y - scrolledYCenter), ripple.type + 1) *
        //         std::sin(ripple.type * 3 + 3 / 4);
        const int distFromCentreSquared =
            std::pow(x - scrolledXCenter, 2) + std::pow(y - scrolledYCenter, 2);
        if (distFromCentreSquared > radius * radius - thickness &&
            distFromCentreSquared < radius * radius)
        {
          currentFrame.AddPixel(x, y, ripple.addColor);
        }
      }
    }
  }
}