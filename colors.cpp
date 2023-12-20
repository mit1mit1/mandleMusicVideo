#include "colors.h"
#include <cmath>
#include <vector>

std::vector<PixelColor> getColors() {
  std::vector<PixelColor> seedColors = {
      PixelColor{.red = 253, .green = 240, .blue = 10, .alpha = 255},
      PixelColor{.red = 252, .green = 197, .blue = 0, .alpha = 255},
      PixelColor{.red = 251, .green = 111, .blue = 4, .alpha = 255},
      PixelColor{.red = 209, .green = 2, .blue = 1, .alpha = 255},
      PixelColor{.red = 33, .green = 53, .blue = 56, .alpha = 255},
      PixelColor{.red = 24, .green = 19, .blue = 162, .alpha = 255},
      PixelColor{.red = 62, .green = 146, .blue = 229, .alpha = 255},
      PixelColor{.red = 99, .green = 228, .blue = 221, .alpha = 255},
      PixelColor{.red = 241, .green = 242, .blue = 248, .alpha = 255},
      PixelColor{.red = 239, .green = 233, .blue = 185, .alpha = 255},
  };

  std::vector<PixelColor> colors = {};
  const int colorsBetween = 6;

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
      intermediateColor.alpha = static_cast<unsigned char>(
          seedColors[i].alpha +
          (seedColors[compareI].alpha - seedColors[i].alpha) * k /
              colorsBetween);
      colors.push_back(intermediateColor);
    }
  }

  return colors;
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
  const int colorJump = 3;
  const float alphaSeed = 0.0;
  alphaModifier = 1.0;

  // TODO: Set alpha based on volume (of particular notes?
  PixelColor color;
  color.alpha = 255;
  if (count >= limit) {
    color.red = color.green = color.blue = 0;
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
    // TODO: Make onsets passed move forwards and backwards ?

    PixelColor previousOffsetColor =
        availableColors[(count + (onsetsPassed - 1) * colorJump) %
                        availableColors.size()];
    PixelColor selectedColor =
        availableColors[(count + onsetsPassed * colorJump) %
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
