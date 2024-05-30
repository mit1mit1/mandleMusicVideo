#ifndef ART_H // To make sure you don't declare the function more than once by
              // including the header multiple times.
#define ART_H

#include "structs.h"
#include <vector>
Ripple getNoteRippleSidescrolling(int minX, int maxX, int minY, int maxY,
                                  AubioNote currentNote,
                                  std::vector<int> minPitches,
                                  std::vector<int> pitchRanges,
                                  int framespersecond, int intrumentNumber);

#endif