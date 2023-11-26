#ifndef MANDLECOLORS_H // To make sure you don't declare the function more than once by including the header multiple times.
#define MANDLECOLORS_H

#include "structs.h"

std::vector<PixelColor> getColors();
PixelColor Palette(int count, int limit, int onsetsPassed, float currentPitch, float previousPitch, int framesSincePitchChange, float alphaModifier, std::vector<PixelColor> availableColors);

#endif
