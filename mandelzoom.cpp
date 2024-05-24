/*
    mandelzoom.cpp

    MIT License

    Copyright (c) 2019 Don Cross <cosinekitty@gmail.com>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions: The
   above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.
*/

#include "aubioParser.h"
#include "colors.h"
#include "lodepng.h"
#include "math.h"
#include "structs.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <vector>

class VideoFrame {
private:
  int width;
  int height;
  std::vector<unsigned char> buffer;

public:
  VideoFrame(int _width, int _height)
      : width(_width), height(_height), buffer(4 * _width * _height, 255) {}

  void BrightenPixel(int x, int y, float multiple) {
    int index = 4 * (y * width + x);
    buffer[index] = (int)(buffer[index] * multiple);
    buffer[index + 1] = (int)(buffer[index + 1] * multiple);
    buffer[index + 2] = (int)(buffer[index + 2] * multiple);
    buffer[index + 3] = (int)(buffer[index + 3] * multiple);

    for (int k = 0; k < 4; ++k) {
      if (buffer[index + k] < 0) {
        buffer[index + k] = 0;
      }
      if (buffer[index + k] > 255) {
        buffer[index + k] = 255;
      }
    }
  }

  void AddPixel(int x, int y, PixelColor color) {
    int index = 4 * (y * width + x);
    buffer[index] = buffer[index] + color.red;
    buffer[index + 1] = buffer[index + 1] + color.green;
    buffer[index + 2] = buffer[index + 2] + color.blue;
    buffer[index + 3] = buffer[index + 3] + color.alpha;

    for (int k = 0; k < 4; ++k) {
      if (buffer[index + k] < 0) {
        buffer[index + k] = 0;
      }
      if (buffer[index + k] > 255) {
        buffer[index + k] = 255;
      }
    }
  }

  void SetPixel(int x, int y, PixelColor color) {
    int index = 4 * (y * width + x);
    buffer[index] = color.red;
    buffer[index + 1] = color.green;
    buffer[index + 2] = color.blue;
    buffer[index + 3] = color.alpha;

    for (int k = 0; k < 4; ++k) {
      if (buffer[index + k] < 0) {
        buffer[index + k] = 0;
      }
      if (buffer[index + k] > 255) {
        buffer[index + k] = 255;
      }
    }
  }

  PixelColor GetPixel(int x, int y) {
    int index = 4 * (y * width + x);
    PixelColor color;
    color.red = buffer[index];
    color.green = buffer[index + 1];
    color.blue = buffer[index + 2];
    color.alpha = buffer[index + 3];
    return color;
  }

  int SavePng(const char *outFileName) {
    unsigned error = lodepng::encode(outFileName, buffer, width, height);
    if (error) {
      fprintf(stderr, "ERROR: lodepng::encode returned %u\n", error);
      return 1;
    }
    return 0;
  }
};

static int PrintUsage();

static int GenerateMandleZoomFrames(const char *outdir, int numframes,
                                    long double xcenter, long double ycenter,
                                    long double zoom, int framespersecond,
                                    std::vector<float> onsetTimestamps,
                                    std::vector<AubioNote> notes);

static int GenerateRippleZoomFrames(const char *outdir, int numframes,
                                    long double xcenter, long double ycenter,
                                    long double zoom, int framespersecond,
                                    std::vector<float> onsetTimestamps,
                                    std::vector<std::vector<AubioNote>> notes);

static double GetTimestampSeconds(int framenumber, int framespersecond);
static AubioNote getCurrentNote(std::vector<AubioNote> notes, float timestamp);

int main(int argc, const char *argv[]) {
  std::cout << std::setprecision(13) << std::fixed;
  if (argc >= 6) {
    const char *outdir = argv[1];
    int numframes = atoi(argv[2]);
    if (numframes < 2) {
      fprintf(stderr, "ERROR: Invalid number of frames on command line. Must "
                      "be at least 2.\n");
      return 1;
    }
    long double xcenter = atof(argv[3]);
    long double ycenter = atof(argv[4]);
    long double zoom = atof(argv[5]);

    int framespersecond = 30;
    if (argc > 6) {
      framespersecond = atoi(argv[6]);
    }
    if (zoom < 1.0) {
      fprintf(stderr, "ERROR: zoom factor must be 1.0 or greater.\n");
      return 1;
    }
    // Add some extra time to fade out the note?
    std::vector<AubioNote> pitchedNotes1 =
        ParseAubioNoteFile("./pitchedInstrument1Notes.txt", 0.0);
    std::vector<AubioNote> pitchedNotes2 =
        ParseAubioNoteFile("./pitchedInstrument2Notes.txt", 0.0);
    std::vector<AubioNote> pitchedNotes3 =
        ParseAubioNoteFile("./pitchedInstrument3Notes.txt", 0.0);
    std::vector<std::vector<AubioNote>> pitchedNotesVec = {
        pitchedNotes1, pitchedNotes2, pitchedNotes3};
    std::vector<float> percussionOnsets =
        ParseOnsetSecondsFile("./rhythmInstrument1Onsets.txt");

    return GenerateRippleZoomFrames(outdir, numframes, xcenter, ycenter, zoom,
                                    framespersecond, percussionOnsets,
                                    pitchedNotesVec);
  }
  return PrintUsage();
}

static int PrintUsage() {
  fprintf(stderr,
          "\n"
          "USAGE:\n"
          "\n"
          "mandelzoom outdir numframes xcenter ycenter zoom\n"
          "    outdir    = directory to receive output PNG files.\n"
          "    numframes = integer number of frames in the video.\n"
          "    xcenter   = the real component of the zoom center point.\n"
          "    ycenter   = the imaginary component of the zoom center point.\n"
          "    zoom      = the magnification factor of the final frame.\n"
          "\n");

  return 1;
}

static double GetTimestampSeconds(int framenumber, int framespersecond) {
  return ((double)framenumber) / ((double)framespersecond);
}

static int GenerateRippleZoomFrames(
    const char *outdir, int numframes, long double xcenter, long double ycenter,
    long double zoom, int framespersecond, std::vector<float> onsetTimestamps,
    std::vector<std::vector<AubioNote>> notesVec) {

  std::vector<PixelColor> availableColors = getColors();
  // Create a video frame buffer with 720p resolution (1280x720).
  PixelColor blankColor;
  blankColor.red = 0;
  blankColor.green = 0;
  blankColor.blue = 0;
  blankColor.alpha = 0;

  VideoFrame currentFrame(xResolution, yResolution);
  for (unsigned int x = 0; x < xResolution; x++) {
    for (unsigned int y = 0; y < yResolution; y++) {
      currentFrame.SetPixel(x, y, blankColor);
    }
  }

  const int startTimeSeconds = 0;
  // Generate the frames
  for (int f = startTimeSeconds * framespersecond; f < numframes; ++f) {
    double timestamp = GetTimestampSeconds(f, framespersecond);

    std::cout << " current timestamp " << timestamp << "\n  ";

    int onsetsPassed = 1;
    for (double onsetTimestamp : onsetTimestamps) {
      if (timestamp > onsetTimestamp) {
        onsetsPassed++;
      }
    }

    std::vector<Ripple> ripples = {};
    for (unsigned int i = 0; i < notesVec.size(); ++i) {
      std::vector<AubioNote> notes = notesVec[i];
      AubioNote currentNote = getCurrentNote(notes, timestamp);
      if (currentNote.startSeconds != -1) {

        std::cout << " setting new ripple at " << timestamp << "\n  ";
        Ripple newRipple;
        newRipple.xCenter =
            (int)((xResolution * (i + 1)) / (notesVec.size() + 1) +
                  (int)currentNote.pitch * 17 % xResolution) -
            xResolution / 2;
        newRipple.yCenter =
            yResolution / 4 + (int)currentNote.pitch / 200 % (yResolution / 2);
        int speedBonus =
            (int)(0.3 / (currentNote.startSeconds - currentNote.endSeconds));
        if (speedBonus > 6) {
          speedBonus = 6;
        }
        newRipple.speed = 3 + speedBonus;
        newRipple.thickness = 90;
        newRipple.startFrame = currentNote.startSeconds * framespersecond;
        PixelColor perimColor1;
        // perimColor1.red = 30;
        // perimColor1.green = 50;
        // perimColor1.blue = 100;
        perimColor1.red =
            (int)((currentNote.pitch *
                   (((i + 1) + onsetsPassed + (i + 1) * onsetsPassed) % 3 +
                    1)) *
                  0.25);
        perimColor1.green =
            (int)((currentNote.pitch *
                   (((i + 1) + onsetsPassed + (i + 1) * onsetsPassed) % 5 +
                    1)) *
                  0.25);
        perimColor1.blue =
            (int)((currentNote.pitch *
                   (((i + 1) + onsetsPassed + (i + 1) * onsetsPassed) % 7 +
                    1)) *
                  0.25);
        perimColor1.alpha = 0;
        newRipple.addColor = perimColor1;
        ripples.push_back(newRipple);
      }
    }
    // }

    for (int x = 0; x < xResolution; ++x) {
      for (int y = 0; y < yResolution; ++y) {
        currentFrame.BrightenPixel(x, y, 0.90);
        for (Ripple ripple : ripples) {
          int framesSinceRippleStart = f - ripple.startFrame;
          int radius = framesSinceRippleStart * ripple.speed + 5;
          int thickness = (ripple.thickness + (radius / 2)) * ripple.thickness +
                          (radius / 2);
          const int distFromCentreSquared =
              (x - ripple.xCenter) * (x - ripple.xCenter) +
              (y - ripple.yCenter) * (y - ripple.yCenter);
          if (distFromCentreSquared > radius * radius - thickness &&
              distFromCentreSquared < radius * radius) {

            currentFrame.AddPixel(x, y, ripple.addColor);
          }
        }
      }
    }

    // Create the output PNG filename in the format "outdir/frame_12345.png".
    char number[20];
    snprintf(number, sizeof(number), "%05d",
             f - startTimeSeconds * framespersecond);
    std::string filename = std::string(outdir) + "/frame_" + number + ".png";

    // Save the video frame as a PNG file.
    int error = currentFrame.SavePng(filename.c_str());
    if (error)
      return error;
  }

  return 0;
}

static AubioNote getCurrentNote(std::vector<AubioNote> notes, float timestamp) {
  bool noteIsPlaying = false;
  // Check changes from pitches
  for (unsigned int i = 0; i < notes.size(); i++) {
    AubioNote checkNote = notes[i];
    if (checkNote.startSeconds < timestamp &&
        checkNote.endSeconds > timestamp) {
      return checkNote;
    }
  }
  AubioNote fakeNote = {};
  fakeNote.endSeconds = -1;
  fakeNote.startSeconds = -1;
  fakeNote.pitch = -1;
  return fakeNote;
}

static int GenerateMandleZoomFrames(const char *outdir, int numframes,
                                    long double xcenter, long double ycenter,
                                    long double zoom, int framespersecond,
                                    std::vector<float> onsetTimestamps,
                                    std::vector<AubioNote> notes) {
  std::vector<PixelColor> availableColors = getColors();
  bool reverseDeadEnd = false;
  int framesSinceDeadEnd = 0;
  int framesSinceLastOnsetPassed = 0;
  const int framesToMoveCentres = 48;
  const double pitchSum = getPitchSum(notes) * 1.0;
  const double averagePitch = pitchSum / notes.size();
  std::cout << " average pitch " << pitchSum << "\n  " << notes.size() << "\n  "
            << averagePitch << "\n  ";
  const double defaultPitch = averagePitch / 600 * (-1);
  bool isSilent = true;
  std::cout << " silent pitch " << defaultPitch;
  // Create a video frame buffer with 720p resolution (1280x720).
  PixelColor blankColor;
  blankColor.red = 0;
  blankColor.green = 0;
  blankColor.blue = 0;
  blankColor.alpha = 0;
  VideoFrame currentFrame(xResolution, yResolution);
  for (unsigned int x = 0; x < xResolution; x++) {
    for (unsigned int y = 0; y < yResolution; y++) {
      currentFrame.SetPixel(x, y, blankColor);
    }
  }

  const int maxLimit = 16000;
  const int minLimit = 1600;
  int limit = minLimit;
  // Below provides a smooth zoom all the way to the specified max zoom
  // double multiplier = pow(zoom, 1.0 / (numframes - 1.0));
  long double smoothMultiplier =
      pow(zoom, 1.0 / (numframes * framespersecond - 1.0));
  std::cout << " initial smooth multiplier " << smoothMultiplier << "\n  ";

  float alphaModifier = 1.0;
  float previousPitch = defaultPitch;
  float currentPitch = defaultPitch;
  float currentNoteLength = 0;
  long double pitchMultiplier = 0.08 * currentPitch / averagePitch;
  long double targetPitchMultiplier = pitchMultiplier;
  std::cout << " initial pitch multiplier " << pitchMultiplier << "\n  ";
  std::cout << " initial target pitch multiplier " << targetPitchMultiplier
            << "\n  ";
  long double denom = 48.0;
  Coordinate nextCentre = {};
  nextCentre.realPart = xcenter;
  nextCentre.imaginaryPart = ycenter;
  int framesSinceChangeOfCentre = 0;
  std::set<int> uniqueMandleCounts;
  int lastOnsetsPassed = 0;

  int mandleCounts[xResolution][yResolution];
  const int startTimeSeconds = 0;
  // Generate the frames
  for (int f = startTimeSeconds * framespersecond; f < numframes; ++f) {
    framesSinceDeadEnd++;
    framesSinceChangeOfCentre++;
    if (limit < maxLimit) {
      limit = limit * 1.012;
    }
    double timestamp = GetTimestampSeconds(f, framespersecond);

    std::cout << " current timestamp " << timestamp << "\n  ";

    long double ver_span = 4.0 / denom;
    long double hor_span = ver_span * (xResolution - 1.0) / (yResolution - 1.0);
    long double ci_top = ycenter + ver_span / 2.0;
    long double yStepDistance = ver_span / (yResolution - 1.0);
    long double cr_left = xcenter - hor_span / 2.0;
    long double xStepDistance = hor_span / (xResolution - 1.0);

    if (framesSinceChangeOfCentre <= framesToMoveCentres) {
      xcenter = xcenter + ((nextCentre.realPart - xcenter) *
                           framesSinceChangeOfCentre / framesToMoveCentres);
      ycenter = ycenter + ((nextCentre.imaginaryPart - ycenter) *
                           framesSinceChangeOfCentre / framesToMoveCentres);
    }

    int onsetsPassed = 1;
    for (double onsetTimestamp : onsetTimestamps) {
      if (timestamp > onsetTimestamp) {
        onsetsPassed++;
      }
    }
    if (onsetsPassed != lastOnsetsPassed) {
      lastOnsetsPassed = onsetsPassed;
      framesSinceLastOnsetPassed = 0;
    } else {
      framesSinceLastOnsetPassed++;
    }

    targetPitchMultiplier = 0.04 * currentPitch / averagePitch;
    pitchMultiplier =
        pitchMultiplier + (targetPitchMultiplier - pitchMultiplier) / 8;

    uniqueMandleCounts.clear();
    if (isSilent) {
      alphaModifier = alphaModifier - 0.025;
    } else {
      alphaModifier = alphaModifier - ((framesSinceChangeOfCentre - 1) * 0.025);
      if (currentNoteLength > 0) {
        alphaModifier =
            (framesSinceChangeOfCentre / framespersecond) / currentNoteLength;
      }
    }
    if (alphaModifier < 0) {
      alphaModifier = 0;
    }
    for (int x = 0; x < xResolution; ++x) {
      long double cr = getXPosition(x, cr_left, xStepDistance);
      for (int y = 0; y < yResolution; ++y) {
        long double ci = getYPosition(y, ci_top, yStepDistance);
        int count = Mandelbrot(cr, ci, limit);
        mandleCounts[x][y] = count;
        uniqueMandleCounts.insert(count);
        PixelColor color = Palette(
            count, limit, onsetsPassed, currentPitch, previousPitch,
            framesSinceChangeOfCentre, framesSinceLastOnsetPassed,
            alphaModifier, availableColors, currentFrame.GetPixel(x, y));
        currentFrame.SetPixel(x, y, color);
      }
    }
    if (uniqueMandleCounts.size() <= 4 &&
        framesSinceDeadEnd > framespersecond) {
      framesSinceDeadEnd = 0;
      std::cout << "!!! Hit dead end - reversing!!!";
      reverseDeadEnd = !reverseDeadEnd;
    }

    // Create the output PNG filename in the format "outdir/frame_12345.png".
    char number[20];
    snprintf(number, sizeof(number), "%05d",
             f - startTimeSeconds * framespersecond);
    std::string filename = std::string(outdir) + "/frame_" + number + ".png";

    // Save the video frame as a PNG file.
    int error = currentFrame.SavePng(filename.c_str());
    if (error)
      return error;

    long double accelerationMultiplier = 0.00005 * f;

    // Increase the zoom magnification for the next frame.
    long double multiplier =
        ((1 + pitchMultiplier + accelerationMultiplier) * smoothMultiplier);
    if (reverseDeadEnd) {
      multiplier = 1 / multiplier;
    }
    denom = denom * multiplier;

    bool noteIsPlaying = false;
    // Check changes from pitches
    for (unsigned int i = 0; i < notes.size(); i++) {
      AubioNote checkNote = notes[i];
      if (checkNote.startSeconds < timestamp &&
          checkNote.endSeconds > timestamp) {
        noteIsPlaying = true;

        if (isSilent || checkNote.pitch != currentPitch) {
          framesSinceChangeOfCentre = 0;
          std::cout << "New note pitch: ";
          std::cout << checkNote.pitch << ", start seconds: ";
          std::cout << checkNote.startSeconds << ", end seconds: ";
          std::cout << checkNote.endSeconds << "\n  ";
          std::cout << "Changed pitch so setting new centre \n";
          previousPitch = currentPitch;
          currentPitch = checkNote.pitch;
          isSilent = false;
          currentNoteLength = checkNote.endSeconds - checkNote.startSeconds;
          // TODO: restrict shorter time notes to smaller intervals - so you
          // don't have to move as fast to get there before the note ends
          int minXIndex = 1 + xResolution / 4;
          int maxXIndex = xResolution - 1 - xResolution / 4;
          int minYIndex = 1 + yResolution / 4;
          int maxYIndex = yResolution - 1 - yResolution / 4;
          std::vector<PixelIndex> interestingPoints =
              getInterestingPixelIndexes(mandleCounts, minXIndex, maxXIndex,
                                         minYIndex, maxYIndex);
          if (interestingPoints.size() > 0) {

            std::cout << nextCentre.realPart << " - next real part  \n  ";
            std::cout << nextCentre.imaginaryPart
                      << " - next imaginary part \n  ";
            std::cout << xcenter << " - current real part  \n  ";
            std::cout << ycenter << " - current imaginary part \n  ";
            Coordinate nextInterstingPoint = chooseRandomInterestingPoint(
                interestingPoints, xStepDistance, yStepDistance, xcenter,
                ycenter, cr_left, ci_top);
            nextCentre.realPart = nextInterstingPoint.realPart;
            nextCentre.imaginaryPart = nextInterstingPoint.imaginaryPart;
          } else {
            std::cout << nextCentre.realPart << "\n  ";
            std::cout << nextCentre.imaginaryPart << " \n  ";
            std::cout << xcenter << " - current real part  \n  ";
            std::cout << ycenter << " - current imaginary part \n  ";
            std::cout << "!!! NO INTERESTING POINTS, RANDOM CHOICE NOT GONNA "
                         "WORK !!!";
          }
        }
        break;
      }
    }
    if (noteIsPlaying == false) {
      isSilent = true;
      if (currentPitch != defaultPitch) {
        previousPitch = currentPitch;
        currentPitch = defaultPitch;
        currentNoteLength = 0;
      }
    }
    if (framesSinceChangeOfCentre > 1) {
      int minXIndex = 1;
      int maxXIndex = xResolution - 1;
      int minYIndex = 1;
      int maxYIndex = yResolution - 1;
      std::vector<PixelIndex> interestingPoints = getInterestingPixelIndexes(
          mandleCounts, minXIndex, maxXIndex, minYIndex, maxYIndex);
      // get interesting point from near target centre if pitch is unchanged
      // (so we constantly add precision as we zoom)
      if (interestingPoints.size() > 0) {
        Coordinate nextInterstingPoint = chooseClosestInterestingPoint(
            interestingPoints, xStepDistance, yStepDistance, xcenter, ycenter,
            nextCentre.realPart, nextCentre.imaginaryPart, cr_left, ci_top);

        std::cout << "Focussing on next interesting point:  \n  ";
        std::cout << nextCentre.realPart << " - next real part  \n  ";
        std::cout << nextCentre.imaginaryPart << " - next imaginary part \n  ";
        std::cout << xcenter << " - current real part  \n  ";
        std::cout << ycenter << " - current imaginary part \n  ";
        nextCentre.realPart = nextInterstingPoint.realPart;
        nextCentre.imaginaryPart = nextInterstingPoint.imaginaryPart;
      } else {
        // TODO: Try lower and lower 'interesting' thresholds
        std::cout << nextCentre.realPart << "\n  ";
        std::cout << nextCentre.imaginaryPart << " \n  ";
        std::cout << xcenter << " - current real part  \n  ";
        std::cout << ycenter << " - current imaginary part \n  ";
        std::cout << "!!! NO INTERESTING POINTS, FOCUS ON EXISTING POINT NOT "
                     "GONNA WORK !!!";
      }
    }
  }
  return 0;
}
