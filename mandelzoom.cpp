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

#include "art.h"
#include "aubioParser.h"
#include "colors.h"
#include "craigsapp-midifile/include/MidiFile.h"
#include "craigsapp-midifile/include/Options.h"
#include "lodepng.h"
#include "math.h"
using namespace smf;
#include "structs.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <vector>

static int PrintUsage();

static int GenerateMandleZoomFrames(const char *outdir, int numframes,
                                    long double xcenter, long double ycenter,
                                    long double zoom, int framespersecond,
                                    std::vector<float> onsetTimestamps,
                                    std::vector<AubioNote> notes);

static int GenerateRippleZoomFrames(
    const char *outdir, int numframes, long double xcenter, long double ycenter,
    long double zoom, int framespersecond, std::vector<float> onsetTimestamps,
    std::vector<std::vector<AubioNote>> aubioNotesVec,
    std::vector<MidiNote> midiNotes);

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

    std::vector<AubioNote> demoAudioNotes =
        ParseAubioNoteFile("./output/demoAudio.txt", 0.0);
    Options options;
    std::vector<std::string> arguments = {"run", "./input/PianoBallad.mid"};
    std::vector<char *> fakeargv;
    for (const auto &arg : arguments)
      fakeargv.push_back((char *)arg.data());
    fakeargv.push_back(nullptr);
    std::cout << "Pushing mid file: " << fakeargv[0] << "\n";
    options.process(fakeargv.size() - 1, fakeargv.data(), 2);
    if (options.getArgCount() != 1) {
      std::cerr << "At least one MIDI filename is required.\n";
      exit(1);
    }
    MidiFile midifile;
    midifile.read(options.getArg(1));
    if (!midifile.status()) {
      std::cerr << "Error reading MIDI file " << options.getArg(1) << std::endl;
      exit(1);
    }
    midifile.joinTracks();
    midifile.doTimeAnalysis();
    midifile.linkNotePairs();

    std::vector<MidiNote> midiNotes = {};

    int track = 0;
    for (int i = 0; i < midifile[track].size(); i++) {
      if (!midifile[track][i].isNoteOn()) {
        continue;
      }
      std::cout << "Note start: " << midifile[track][i].seconds
                << "; duration: " << midifile[track][i].getDurationInSeconds()
                << "; P1 (pitch?): " << midifile[track][i].getP1()
                << "; P2 (pitch?): " << midifile[track][i].getP2()
                << "; P3 (pitch?): " << midifile[track][i].getP3() << '\t'
                << "Track identifier?: " << midifile[track][i][1] << std::endl;
      MidiNote newNote;

      newNote.pitch = midifile[track][i].getP1();
      newNote.volume = midifile[track][i].getP2();
      newNote.startSeconds = midifile[track][i].seconds;
      newNote.endSeconds = midifile[track][i].seconds +
                           midifile[track][i].getDurationInSeconds();
      newNote.trackNumber = i;
      midiNotes.push_back(newNote);
    }
    // std::vector<AubioNote> pitchedNotes1 =
    //     ParseAubioNoteFile("./output/pitchedInstrument1Notes.txt", 0.0);
    // std::vector<AubioNote> pitchedNotes2 =
    //     ParseAubioNoteFile("./output/pitchedInstrument2Notes.txt", 0.0);
    // std::vector<AubioNote> pitchedNotes3 =
    //     ParseAubioNoteFile("./output/pitchedInstrument3Notes.txt", 0.0);
    // std::vector<AubioNote> pitchedNotes4 =
    //     ParseAubioNoteFile("./output/pitchedInstrument1Notes.txt", 0.0);
    // std::vector<AubioNote> pitchedNotes5 =
    //     ParseAubioNoteFile("./output/pitchedInstrument2Notes.txt", 0.0);
    // std::vector<AubioNote> pitchedNotes6 =
    //     ParseAubioNoteFile("./output/pitchedInstrument3Notes.txt", 0.0);
    // std::vector<AubioNote> pitchedNotes7 =
    //     ParseAubioNoteFile("./output/pitchedInstrument3Notes.txt", 0.0);
    std::vector<std::vector<AubioNote>> pitchedNotesVec = {};
    std::vector<float> percussionOnsets =
        ParseOnsetSecondsFile("./output/rhythmInstrument1Onsets.txt");

    return GenerateRippleZoomFrames(outdir, numframes, xcenter, ycenter, zoom,
                                    framespersecond, percussionOnsets,
                                    pitchedNotesVec, midiNotes);
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
    std::vector<std::vector<AubioNote>> aubioNotesVec,
    std::vector<MidiNote> midiNotes) {

  std::vector<int> aubioMaxPitches{};
  std::vector<int> aubioMinPitches{};
  std::vector<int> aubioPitchRanges{};
  for (unsigned int i = 0; i < aubioNotesVec.size(); i++) {
    aubioMaxPitches.push_back(getMaxPitch(aubioNotesVec[i]));
    aubioMinPitches.push_back(getMinPitch(aubioNotesVec[i]));
    aubioPitchRanges.push_back((aubioMaxPitches[i] - aubioMinPitches[i]));
  }

  std::vector<PixelColor> availableColors = getColors();
  // Create a video frame buffer with 720p resolution (1280x720).
  PixelColor blankColor;
  blankColor.red = 0;
  blankColor.green = 0;
  blankColor.blue = 0;
  blankColor.alpha = 255;
  PixelColor backgroundColor;
  backgroundColor.red = 0;
  backgroundColor.green = 0;
  backgroundColor.blue = 0;
  backgroundColor.alpha = 255;
  const int backgroundColorMaxSaturation = 20;
  const double onsetColorChangeLength = 0.4;

  const int scrollSpeedX = 0;
  const int scrollSpeedY = 0;

  const double spinSpeedRadiansPerFrame = 0.01;
  const double zoomMultiplierPerFrame = 1.01;

  VideoFrame currentFrame(xResolution, yResolution);
  currentFrame.SetAllPixels(backgroundColor);

  const int startTimeSeconds = 0;
  // Generate the frames
  for (int f = startTimeSeconds * framespersecond; f < numframes; ++f) {
    double timestamp = GetTimestampSeconds(f, framespersecond);

    std::cout << " current timestamp " << timestamp << "\n  ";

    int onsetsPassed = 1;
    double lastOnsetTimestamp = -2 * onsetColorChangeLength;
    for (double onsetTimestamp : onsetTimestamps) {
      if (timestamp > onsetTimestamp) {
        onsetsPassed++;
        lastOnsetTimestamp = onsetTimestamp;
      }
    }

    // TODO: Fix this doing color jumps
    // backgroundColor = getNextBackgroundColor(
    //     timestamp, lastOnsetTimestamp, onsetColorChangeLength,
    //     onsetsPassed, backgroundColor, backgroundColorMaxSaturation);

    currentFrame.ScrollPixels(scrollSpeedX, scrollSpeedY, blankColor);
    currentFrame.SpinZoomPixels(spinSpeedRadiansPerFrame,
                                zoomMultiplierPerFrame, blankColor);

    std::vector<Ripple> ripples = {};
    for (unsigned int i = 0; i < aubioNotesVec.size(); ++i) {
      std::vector<AubioNote> notes = aubioNotesVec[i];
      AubioNote currentNote = getCurrentNote(notes, timestamp);
      if (currentNote.startSeconds != -1) {

        // std::cout << " setting new ripple at " << timestamp << "\n  ";
        Ripple newRipple = getNoteRippleCircleOfScales(
            xResolution, yResolution, currentNote.pitch,
            currentNote.startSeconds, currentNote.endSeconds, framespersecond,
            i);

        // Ripple newRipple = getNoteRippleSidescrolling(
        //     0, xResolution - 1, 0, yResolution - 1, currentNote, minPitches,
        //     pitchRanges, framespersecond, i);
        ripples.push_back(newRipple);
      }
    }

    for (unsigned int i = 0; i < midiNotes.size(); ++i) {
      MidiNote checkNote = midiNotes[i];
      // Never check this note again if we're already past it's end time
      if (checkNote.endSeconds < timestamp) {
        midiNotes.erase(midiNotes.begin() + i);
        continue;
      }
      if (checkNote.startSeconds <= timestamp &&
          checkNote.endSeconds >= timestamp) {

        // std::cout << " setting new ripple at " << timestamp << "\n  ";
        Ripple newRipple = getNoteRippleCircleOfScales(
            xResolution, yResolution, checkNote.pitch, checkNote.startSeconds,
            checkNote.endSeconds, framespersecond, i);

        // Ripple newRipple = getNoteRippleSidescrolling(
        //     0, xResolution - 1, 0, yResolution - 1, currentNote, minPitches,
        //     pitchRanges, framespersecond, i);
        ripples.push_back(newRipple);
      }
    }

    currentFrame.BrightenAllPixels(0.75);

    for (Ripple ripple : ripples) {
      int framesSinceRippleStart = f - ripple.startFrame;
      int radius = framesSinceRippleStart * ripple.speed + 12;
      int thickness =
          (ripple.thickness + (radius / 2)) * ripple.thickness + (radius / 2);
      // TODO: Extract discrete zoom and use it to move ripple centre as
      // well
      Coordinate zoomDiff = getDiscreteZoomDiff(
          ripple.xCenter, ripple.yCenter, xResolution / 2, yResolution / 2);
      int scrolledXCenter =
          ripple.xCenter -
          framesSinceRippleStart * (scrollSpeedX + zoomDiff.realPart);
      int scrolledYCenter =
          ripple.yCenter - framesSinceRippleStart *
                               (scrollSpeedY + zoomDiff.imaginaryPart);
      for (int x = 0; x < xResolution; ++x) {
        for (int y = 0; y < yResolution; ++y) {
          const int distFromCentreSquared =
              (x - scrolledXCenter) * (x - scrolledXCenter) +
              (y - scrolledYCenter) * (y - scrolledYCenter);
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
  blankColor.alpha = 255;
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

    // std::cout << " current timestamp " << timestamp << "\n  ";

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
