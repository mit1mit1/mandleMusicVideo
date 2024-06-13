#include "colors.h"
#include <cmath>
#include <vector>

std::vector<PixelColor> getColors() {
  std::vector<PixelColor> seedColors = {
      PixelColor{.red = 0, .green = 0, .blue = 0, .alpha = 255},
      PixelColor{.red = 255, .green = 224, .blue = 146, .alpha = 255},
      PixelColor{.red = 255, .green = 243, .blue = 248, .alpha = 255},
      PixelColor{.red = 255, .green = 224, .blue = 146, .alpha = 255},
      PixelColor{.red = 0, .green = 0, .blue = 0, .alpha = 255},
  };

  std::vector<PixelColor> colors = {};
  const int colorsBetween = 32;

  for (unsigned int i = 0; i < seedColors.size(); i++) {
    unsigned int compareI = i + 1;
    colors.push_back(seedColors[i]);
    if (compareI >= seedColors.size()) {
      compareI = 0;
    }
    for (int k = 1; k < colorsBetween; k++) {
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

  return colors;
}

static double ZigZag(double x) {
  double y = fmod(fabs(x), 2.0);
  if (y > 1.0)
    y = 1.0 - y;
  return y;
}

static int getWavyOnsets(int onsetsPassed) {
  if (onsetsPassed % 8 == 0) {
    return onsetsPassed - 1;
  }
  if (onsetsPassed % 8 == 1) {
    return onsetsPassed - 3;
  }
  if (onsetsPassed % 8 == 2) {
    return onsetsPassed - 5;
  }
  if (onsetsPassed % 8 == 4) {
    return onsetsPassed - 4;
  }
  if (onsetsPassed % 8 == 5) {
    return onsetsPassed - 3;
  }
  if (onsetsPassed % 8 == 6) {
    return onsetsPassed - 1;
  }
  return onsetsPassed;
}

// TODO more create colors - e.g. setting pallette to a length of 12
// setting the first three elements based on track 1's pitch,
// the next three based on track 2's pitch,
// the third three based on track 3's pitch,
// and the final 3 based on percussion onsets passed.

PixelColor Palette(int count, int limit, int onsetsPassed, float currentPitch,
                   float previousPitch, int framesSincePitchChange,
                   int framesSinceLastOnsetPassed, float alphaModifier,
                   std::vector<PixelColor> availableColors,
                   PixelColor currentColor) {
  const int onsetColorJump = 3;
  const float alphaSeed = 0.0;
  alphaModifier = 1.0;

  onsetsPassed = getWavyOnsets(onsetsPassed);

  // TODO: Set alpha based on volume (of particular notes?
  PixelColor color;
  color.alpha = 255;
  if (count >= limit) {
    color.green = color.blue = 5;
    double red = 50.0 * ZigZag(0.0342 * onsetsPassed);
    if (red > 50.0) {
      red = fmod(red, 50.0);
    }
    if (red > 0.0) {
      red = 0.0;
    }
    color.red = static_cast<unsigned char>(red);
  } else {
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
    float currentAlphaModifier = 1 - (count * currentPitch * alphaSeed -
                                      floor(count * currentPitch * alphaSeed));
    // float bonusAlphaModifier =
    //     previousBonusAlphaModified +
    //     (currentAlphaModifier - previousBonusAlphaModified) *
    //         framesSincePitchChange / framesToChangeFade;

    PixelColor previousOffsetColor =
        availableColors[(count + (onsetsPassed - 1) * onsetColorJump) %
                        availableColors.size()];
    PixelColor selectedColor =
        availableColors[(count + onsetsPassed * onsetColorJump) %
                        availableColors.size()];

    float smoothColorChangeRatio = framesSinceLastOnsetPassed * 0.2;
    if (smoothColorChangeRatio > 1.0) {
      smoothColorChangeRatio = 1.0;
    }
    if (smoothColorChangeRatio < 0.0) {
      smoothColorChangeRatio = 0.0;
    }
    selectedColor.red = static_cast<unsigned char>(
        selectedColor.red * smoothColorChangeRatio +
        previousOffsetColor.red * (1.0 - smoothColorChangeRatio));
    selectedColor.green = static_cast<unsigned char>(
        selectedColor.green * smoothColorChangeRatio +
        previousOffsetColor.green * (1.0 - smoothColorChangeRatio));
    selectedColor.blue = static_cast<unsigned char>(
        selectedColor.blue * smoothColorChangeRatio +
        previousOffsetColor.blue * (1.0 - smoothColorChangeRatio));

    if (alphaModifier >= 1 || alphaModifier <= 0) {
      alphaModifier = 1.0;
    }
    // alphaModifier = alphaModifier;
    float blurRatio = 1.0;
    color.red = static_cast<unsigned char>(
        currentColor.red * (1.0 - blurRatio) +
        (selectedColor.red * alphaModifier) * blurRatio);
    color.green = static_cast<unsigned char>(
        currentColor.green * (1.0 - blurRatio) +
        (selectedColor.green * alphaModifier) * blurRatio);
    color.blue = static_cast<unsigned char>(
        currentColor.blue * (1.0 - blurRatio) +
        (selectedColor.blue * alphaModifier) * blurRatio);
  }

  return color;
}
