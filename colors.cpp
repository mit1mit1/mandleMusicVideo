#include "colors.h"
#include <cmath>
#include <vector>

std::vector<PixelColor> getColors() {
  std::vector<PixelColor> rollingInTheDeepColors = {
      PixelColor{.red = 232, .green = 208, .blue = 197, .alpha = 255},
      PixelColor{.red = 208, .green = 173, .blue = 167, .alpha = 255},
      PixelColor{.red = 173, .green = 106, .blue = 108, .alpha = 255},
      PixelColor{.red = 93, .green = 46, .blue = 70, .alpha = 255},
      PixelColor{.red = 181, .green = 141, .blue = 182, .alpha = 255},
      PixelColor{.red = 197, .green = 175, .blue = 160, .alpha = 255},
      PixelColor{.red = 233, .green = 188, .blue = 183, .alpha = 255},
      PixelColor{.red = 171, .green = 117, .blue = 107, .alpha = 255},
      PixelColor{.red = 0, .green = 129, .blue = 175, .alpha = 255},
      PixelColor{.red = 0, .green = 171, .blue = 231, .alpha = 255},
      PixelColor{.red = 45, .green = 199, .blue = 255, .alpha = 255},
      PixelColor{.red = 6, .green = 7, .blue = 14, .alpha = 255},
      PixelColor{.red = 41, .green = 82, .blue = 74, .alpha = 255},
      PixelColor{.red = 148, .green = 161, .blue = 135, .alpha = 255},
      PixelColor{.red = 147, .green = 172, .blue = 181, .alpha = 255},
      PixelColor{.red = 150, .green = 197, .blue = 247, .alpha = 255},
      PixelColor{.red = 169, .green = 211, .blue = 255, .alpha = 255},
      PixelColor{.red = 242, .green = 244, .blue = 255, .alpha = 255},
      PixelColor{.red = 234, .green = 210, .blue = 234, .alpha = 255},
      PixelColor{.red = 234, .green = 186, .blue = 107, .alpha = 255},
      PixelColor{.red = 138, .green = 176, .blue = 171, .alpha = 255},
      PixelColor{.red = 62, .green = 80, .blue = 91, .alpha = 255},
      PixelColor{.red = 38, .green = 65, .blue = 60, .alpha = 255},
      PixelColor{.red = 26, .green = 29, .blue = 26, .alpha = 255},
      PixelColor{.red = 3, .green = 18, .blue = 14, .alpha = 255},
  };

  std::vector<PixelColor> colors = {};
  const int colorsBetween = 32;

  for (unsigned int i = 0; i < rollingInTheDeepColors.size(); i++) {
    unsigned int compareI = i + 1;
    colors.push_back(rollingInTheDeepColors[i]);
    if (compareI >= rollingInTheDeepColors.size()) {
      compareI = 0;
    }
    for (int k = 1; k < colorsBetween; k++) {
      PixelColor intermediateColor;
      intermediateColor.red =
          static_cast<unsigned char>(rollingInTheDeepColors[i].red +
                                     (rollingInTheDeepColors[compareI].red -
                                      rollingInTheDeepColors[i].red) *
                                         k / colorsBetween);
      intermediateColor.green =
          static_cast<unsigned char>(rollingInTheDeepColors[i].green +
                                     (rollingInTheDeepColors[compareI].green -
                                      rollingInTheDeepColors[i].green) *
                                         k / colorsBetween);
      intermediateColor.blue =
          static_cast<unsigned char>(rollingInTheDeepColors[i].blue +
                                     (rollingInTheDeepColors[compareI].blue -
                                      rollingInTheDeepColors[i].blue) *
                                         k / colorsBetween);
      intermediateColor.alpha =
          static_cast<unsigned char>(rollingInTheDeepColors[i].alpha +
                                     (rollingInTheDeepColors[compareI].alpha -
                                      rollingInTheDeepColors[i].alpha) *
                                         k / colorsBetween);
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
                   float alphaModifier, std::vector<PixelColor> availableColors,
                   PixelColor currentColor) {
  // TODO: Set alpha based on volume (of particular notes?
  PixelColor color;
  color.alpha = 255;
  if (count >= limit) {
    color.red = color.green = color.blue = 0;
  } else {
    // TODO: Fade different colors based on what note
    // TODO: Keep track of previous fade, and slowly transition fade (so can
    // increase to 90% fade without triggering epilepsy)
    const int framesToChangeFade = 6;
    if (framesSincePitchChange > framesToChangeFade) {
      framesSincePitchChange = framesToChangeFade;
    }
    float previousBonusAlphaModified =
        1 - (count * previousPitch * 0.593284783 -
             floor(count * previousPitch * 0.593284783)) *
                0.3;
    float currentAlphaModifier =
        1 - (count * currentPitch * 0.593284783 -
             floor(count * currentPitch * 0.593284783)) *
                0.3;
    float bonusAlphaModifier =
        previousBonusAlphaModified +
        (currentAlphaModifier - previousBonusAlphaModified) *
            framesSincePitchChange / framesToChangeFade;
    PixelColor selectedColor =
        availableColors[(count + onsetsPassed * 7) % availableColors.size()];
    if (alphaModifier >= 1 || alphaModifier <= 0) {
      alphaModifier = 1.0;
    }
    alphaModifier = bonusAlphaModifier * alphaModifier;
    alphaModifier = alphaModifier;
    color.red = static_cast<unsigned char>(
        currentColor.red +
        (currentColor.red - selectedColor.red * alphaModifier) * 0.5);
    color.green = static_cast<unsigned char>(
        currentColor.green +
        (currentColor.green - selectedColor.green * alphaModifier) * 0.5);
    color.blue = static_cast<unsigned char>(
        currentColor.blue +
        (currentColor.blue - selectedColor.blue * alphaModifier) * 0.5);
  }

  return color;
}
