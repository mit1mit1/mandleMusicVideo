#include "art.h" // art.h must be in the current directory. or use relative or absolute path to it. e.g #include "math/player.h"
#include "structs.h"
#include <vector>

Ripple getNoteRippleSidescrolling(int minX, int maxX, int minY, int maxY,
                     AubioNote currentNote, std::vector<int> minPitches,
                     std::vector<int> pitchRanges, int framespersecond,
                     int intrumentNumber) {
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