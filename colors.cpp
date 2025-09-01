#include "colors.h"
#include <cmath>
#include <vector>

std::vector<PixelColor> getColors()
{
  // Golden
  // std::vector<PixelColor> seedColors = {
  //     PixelColor{.red = 0, .green = 0, .blue = 0, .alpha = 255},
  //     PixelColor{.red = 255, .green = 224, .blue = 146, .alpha = 255},
  //     PixelColor{.red = 255, .green = 243, .blue = 248, .alpha = 255},
  //     PixelColor{.red = 255, .green = 224, .blue = 146, .alpha = 255},
  //     PixelColor{.red = 0, .green = 0, .blue = 0, .alpha = 255},
  // };

  // Disco snails
  // std::vector<PixelColor> seedColors = {
  //     PixelColor{.red = 239, .green = 245, .blue = 255, .alpha = 255},
  //     PixelColor{.red = 164, .green = 168, .blue = 241, .alpha = 255},
  //     PixelColor{.red = 239, .green = 245, .blue = 255, .alpha = 255},
  //     PixelColor{.red = 177, .green = 181, .blue = 228, .alpha = 255},
  //     PixelColor{.red = 81, .green = 51, .blue = 173, .alpha = 255},
  //     PixelColor{.red = 177, .green = 181, .blue = 228, .alpha = 255},
  //     PixelColor{.red = 239, .green = 245, .blue = 255, .alpha = 255},
  //     PixelColor{.red = 177, .green = 181, .blue = 228, .alpha = 255},
  //     PixelColor{.red = 252, .green = 96, .blue = 161, .alpha = 255},
  //     PixelColor{.red = 81, .green = 51, .blue = 173, .alpha = 255},
  //     PixelColor{.red = 38, .green = 246, .blue = 171, .alpha = 255},
  //     PixelColor{.red = 81, .green = 51, .blue = 173, .alpha = 255},
  //     PixelColor{.red = 177, .green = 181, .blue = 228, .alpha = 255},
  //     PixelColor{.red = 239, .green = 245, .blue = 255, .alpha = 255},
  //     PixelColor{.red = 177, .green = 181, .blue = 228, .alpha = 255},
  //     PixelColor{.red = 81, .green = 51, .blue = 173, .alpha = 255},
  //     PixelColor{.red = 239, .green = 245, .blue = 255, .alpha = 255},
  //     PixelColor{.red = 81, .green = 51, .blue = 173, .alpha = 255},
  //     PixelColor{.red = 255, .green = 111, .blue = 251, .alpha = 255},
  //     PixelColor{.red = 151, .green = 152, .blue = 235, .alpha = 255},
  //     PixelColor{.red = 239, .green = 245, .blue = 255, .alpha = 255},
  // };

  // Sand Pallette
  std::vector<PixelColor> seedColors = {
      PixelColor{.red = 234, .green = 177, .blue = 122, .alpha = 255},
      PixelColor{.red = 238, .green = 237, .blue = 245, .alpha = 255},
      PixelColor{.red = 227, .green = 152, .blue = 59, .alpha = 255},
      PixelColor{.red = 217, .green = 150, .blue = 80, .alpha = 255},
      PixelColor{.red = 227, .green = 152, .blue = 59, .alpha = 255},
      PixelColor{.red = 238, .green = 237, .blue = 245, .alpha = 255},
  };

  // // Autumn Pallette
  // std::vector<PixelColor> seedColors = {
  //     // Macadamia
  //     PixelColor{.red = 238, .green = 224, .blue = 200, .alpha = 255},
  //     // Dark Ginger
  //     PixelColor{.red = 172, .green = 92, .blue = 56, .alpha = 255},
  //     // Amber
  //     PixelColor{.red = 214, .green = 120, .blue = 52, .alpha = 255},
  //     // Laguna 2
  //     // PixelColor{.red = 0, .green = 100, .blue = 118, .alpha = 255},
  //     // Brick red
  //     PixelColor{.red = 211, .green = 58, .blue = 52, .alpha = 255},
  //     // Olive
  //     // PixelColor{.red = 147, .green = 152, .blue = 94, .alpha = 255},
  //     // Camel
  //     PixelColor{.red = 180, .green = 124, .blue = 65, .alpha = 255},
  //     // Laguna 1
  //     // PixelColor{.red = 29, .green = 90, .blue = 107, .alpha = 255},
  //     // Soft Olive
  //     // PixelColor{.red = 125, .green = 123, .blue = 79, .alpha = 255},
  //     // Mustard
  //     PixelColor{.red = 230, .green = 176, .blue = 81, .alpha = 255},
  //     // Chocolate
  //     PixelColor{.red = 50, .green = 39, .blue = 19, .alpha = 255},
  //     // Spiced Coral
  //     PixelColor{.red = 229, .green = 110, .blue = 104, .alpha = 255},
  // };
  std::vector<PixelColor>
      colors = {};
  const int colorsBetween = 96;

  for (unsigned int i = 0; i < seedColors.size(); i++)
  {
    unsigned int compareI = i + 1;
    colors.push_back(seedColors[i]);
    if (compareI >= seedColors.size())
    {
      compareI = 0;
    }
    for (int k = 1; k < colorsBetween; k++)
    {
      PixelColor intermediateColor;
      intermediateColor.red = static_cast<unsigned char>(
          seedColors[i].red +
          (seedColors[compareI].red - seedColors[i].red) * k / colorsBetween);
      intermediateColor.green = static_cast<unsigned char>(
          seedColors[i].green +
          (seedColors[compareI].green - seedColors[i].green) * k /
              colorsBetween);
      intermediateColor.blue = static_cast<unsigned char>(
          seedColors[i].blue +
          (seedColors[compareI].blue - seedColors[i].blue) * k / colorsBetween);
      // intermediateColor.alpha = static_cast<unsigned char>(
      //     seedColors[i].alpha +
      //     (seedColors[compareI].alpha - seedColors[i].alpha) * k /
      //         colorsBetween);
      intermediateColor.alpha = 255;
      colors.push_back(intermediateColor);
    }
  }

  const int oneWayColorSize = colors.size();
  for (unsigned int i = 1; i < oneWayColorSize - 1; i++)
  {
    colors.push_back(colors[oneWayColorSize - i]);
  }

  return colors;
}

static double ZigZag(double x)
{
  double y = fmod(fabs(x), 2.0);
  if (y > 1.0)
    y = 1.0 - y;
  return y;
}

static int getWavyOnsets(int onsetsPassed)
{
  if (onsetsPassed % 8 == 0)
  {
    return onsetsPassed - 1;
  }
  if (onsetsPassed % 8 == 1)
  {
    return onsetsPassed - 3;
  }
  if (onsetsPassed % 8 == 2)
  {
    return onsetsPassed - 5;
  }
  if (onsetsPassed % 8 == 4)
  {
    return onsetsPassed - 4;
  }
  if (onsetsPassed % 8 == 5)
  {
    return onsetsPassed - 3;
  }
  if (onsetsPassed % 8 == 6)
  {
    return onsetsPassed - 1;
  }
  return onsetsPassed;
}

// TODO more create colors - e.g. setting pallette to a length of 12
// setting the first three elements based on track 1's pitch,
// the next three based on track 2's pitch,
// the third three based on track 3's pitch,
// and the final 3 based on percussion onsets passed.

// TODO compute all this per frame instead of per pixel (actually some of it can just be once at start)
PixelColor Palette(int count, int limit, int onsetsPassed, float currentPitch,
                   float previousPitch, int framesSincePitchChange,
                   int framesSinceLastOnsetPassed, float alphaModifier,
                   std::vector<PixelColor> availableColors, PixelColor blankColor, const int onsetColorJump)
{
  const float alphaSeed = 0.0;
  alphaModifier = 1.0;
  int previousColorIndex = (count + (onsetsPassed - 1) * onsetColorJump);
  int colorIndex = (count + onsetsPassed * onsetColorJump);

  // onsetsPassed = getWavyOnsets(onsetsPassed);

  // TODO: Set alpha based on volume (of particular notes?
  if (count >= limit)
  {
    // return blankColor;

    // color.green = std::min(std::max((int)(blankColor.green * (1 - ((onsetsPassed) % 10 - 10) * 0.01)), 0), 255);
    // color.blue = std::min(std::max((int)(blankColor.blue * (1 - ((onsetsPassed) % 10 - 10) * 0.01)), 0), 255);
    // color.red = std::min(std::max((int)(blankColor.red * (1 - ((onsetsPassed) % 10 - 10) * 0.01)), 0), 255);
    // color.alpha = blankColor.alpha;

    // color.green = std::min(std::max((currentColor.green + 5 * blankColor.green) / 6, 0), 255);
    // color.blue = std::min(std::max((currentColor.blue + 5 * blankColor.blue) / 6, 0), 255);
    // color.red = std::min(std::max((currentColor.red + 5 * blankColor.red) / 6, 0), 255);
    // color.alpha = std::min(std::max((currentColor.alpha + 5 * blankColor.alpha) / 6, 0), 255);
    // color.green = color.blue = 5;
    // double red = 50.0 * ZigZag(0.0342 * onsetsPassed);
    // if (red > 50.0)
    // {
    //   red = fmod(red, 50.0);
    // }
    // if (red > 0.0)
    // {
    //   red = 0.0;
    // }
    // color.red = static_cast<unsigned char>(red);
  }
  // TODO: Fade different colors based on what note
  // TODO: Keep track of previous fade, and slowly transition fade (so can
  // increase to 90% fade without triggering epilepsy)
  // const int framesToChangeFade = 6;
  // if (framesSincePitchChange > framesToChangeFade) {
  //   framesSincePitchChange = framesToChangeFade;
  // }
  // float previousBonusAlphaModified =
  //     1 - (count * previousPitch * 0.593284783 -
  //          floor(count * previousPitch * 0.593284783));
  // float currentAlphaModifier = 1 - (count * currentPitch * alphaSeed -
  //                                   floor(count * currentPitch * alphaSeed));
  // float bonusAlphaModifier =
  //     previousBonusAlphaModified +
  //     (currentAlphaModifier - previousBonusAlphaModified) *
  //         framesSincePitchChange / framesToChangeFade;

  // PixelColor previousOffsetColor =
  //     availableColors[previousColorIndex %
  //                     availableColors.size()];
  PixelColor selectedColor =
      availableColors[colorIndex %
                      availableColors.size()];
  return selectedColor;
  // Weird smoothing
  // float smoothColorChangeRatio = framesSinceLastOnsetPassed * 0.2;
  // if (smoothColorChangeRatio > 1.0)
  // {
  //   smoothColorChangeRatio = 1.0;
  // }
  // if (smoothColorChangeRatio < 0.0)
  // {
  //   smoothColorChangeRatio = 0.0;
  // }

  // selectedColor.red = static_cast<unsigned char>(
  //     selectedColor.red * smoothColorChangeRatio +
  //     previousOffsetColor.red * (1.0 - smoothColorChangeRatio));
  // selectedColor.green = static_cast<unsigned char>(
  //     selectedColor.green * smoothColorChangeRatio +
  //     previousOffsetColor.green * (1.0 - smoothColorChangeRatio));
  // selectedColor.blue = static_cast<unsigned char>(
  //     selectedColor.blue * smoothColorChangeRatio +
  //     previousOffsetColor.blue * (1.0 - smoothColorChangeRatio));

  //   if (alphaModifier >= 1 || alphaModifier <= 0)
  //   {
  //     alphaModifier = 1.0;
  //   }
  //   // alphaModifier = alphaModifier;
  //   float blurRatio = 1.0;
  //   color.red = static_cast<unsigned char>(
  //       currentColor.red * (1.0 - blurRatio) +
  //       (selectedColor.red * alphaModifier) * blurRatio);
  //   color.green = static_cast<unsigned char>(
  //       currentColor.green * (1.0 - blurRatio) +
  //       (selectedColor.green * alphaModifier) * blurRatio);
  //   color.blue = static_cast<unsigned char>(
  //       currentColor.blue * (1.0 - blurRatio) +
  //       (selectedColor.blue * alphaModifier) * blurRatio);
}
