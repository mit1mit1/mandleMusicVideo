#include "colors.h"
#include <cmath>
#include <vector>

std::vector<PixelColor> getColors() {
  std::vector<PixelColor> drWhoColors = {
      PixelColor{.red = 255, .green = 237, .blue = 231, .alpha = 255},
      PixelColor{.red = 254, .green = 228, .blue = 213, .alpha = 255},
      PixelColor{.red = 242, .green = 146, .blue = 147, .alpha = 255},
      PixelColor{.red = 197, .green = 86, .blue = 137, .alpha = 255},
      PixelColor{.red = 142, .green = 107, .blue = 187, .alpha = 255},
      PixelColor{.red = 87, .green = 46, .blue = 140, .alpha = 255},
      PixelColor{.red = 69, .green = 26, .blue = 142, .alpha = 255},
      PixelColor{.red = 143, .green = 25, .blue = 111, .alpha = 255},
      PixelColor{.red = 200, .green = 117, .blue = 135, .alpha = 255},
      PixelColor{.red = 254, .green = 238, .blue = 215, .alpha = 255},
      PixelColor{.red = 146, .green = 115, .blue = 182, .alpha = 255},
      PixelColor{.red = 178, .green = 96, .blue = 180, .alpha = 255},
    
  };

  std::vector<PixelColor> colors = {};
  const int colorsBetween = 32;

  for (unsigned int i = 0; i < drWhoColors.size(); i++) {
    unsigned int compareI = i + 1;
    colors.push_back(drWhoColors[i]);
    if (compareI >= drWhoColors.size()) {
      compareI = 0;
    }
    for (int k = 1; k < colorsBetween; k++) {
      PixelColor intermediateColor;
      intermediateColor.red =
          static_cast<unsigned char>(drWhoColors[i].red +
                                     (drWhoColors[compareI].red -
                                      drWhoColors[i].red) *
                                         k / colorsBetween);
      intermediateColor.green =
          static_cast<unsigned char>(drWhoColors[i].green +
                                     (drWhoColors[compareI].green -
                                      drWhoColors[i].green) *
                                         k / colorsBetween);
      intermediateColor.blue =
          static_cast<unsigned char>(drWhoColors[i].blue +
                                     (drWhoColors[compareI].blue -
                                      drWhoColors[i].blue) *
                                         k / colorsBetween);
      intermediateColor.alpha =
          static_cast<unsigned char>(drWhoColors[i].alpha +
                                     (drWhoColors[compareI].alpha -
                                      drWhoColors[i].alpha) *
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
    // float previousBonusAlphaModified =
    //     1 - (count * previousPitch * 0.593284783 -
    //          floor(count * previousPitch * 0.593284783));
    float currentAlphaModifier =
        1 - (count * currentPitch * 0.593284783 -
             floor(count * currentPitch * 0.593284783));
    // float bonusAlphaModifier =
    //     previousBonusAlphaModified +
    //     (currentAlphaModifier - previousBonusAlphaModified) *
    //         framesSincePitchChange / framesToChangeFade;
    PixelColor selectedColor =
        availableColors[(count + onsetsPassed * 7) % availableColors.size()];
    if (alphaModifier >= 1 || alphaModifier <= 0) {
      alphaModifier = 1.0;
    }
    // alphaModifier = alphaModifier;
    color.red = static_cast<unsigned char>(
        currentColor.red * 0.9 + (selectedColor.red * alphaModifier) * 0.1);
    color.green = static_cast<unsigned char>(
        currentColor.green * 0.9 + (selectedColor.green * alphaModifier) * 0.1);
    color.blue = static_cast<unsigned char>(
        currentColor.blue * 0.9 + (selectedColor.blue * alphaModifier) * 0.1);
  }

  return color;
}
