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
#include <string>
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
                                    std::vector<std::vector<AubioNote>> aubioNotesVec,
                                    std::vector<MidiNote> midiNotes, std::vector<MidiNote> midiPercussionNotes);

static int GenerateRippleZoomFrames(
    const char *outdir, int numframes, long double xcenter, long double ycenter,
    long double zoom, int framespersecond, std::vector<float> onsetTimestamps,
    std::vector<std::vector<AubioNote>> aubioNotesVec,
    std::vector<MidiNote> midiNotes);

static double GetTimestampSeconds(int framenumber, int framespersecond);
static AubioNote getCurrentNote(std::vector<AubioNote> notes, float timestamp);

int main(int argc, const char *argv[])
{
  std::cout << std::setprecision(13) << std::fixed;
  if (argc >= 6)
  {
    const char *outdir = argv[1];
    int numframes = atoi(argv[2]);
    if (numframes < 2)
    {
      fprintf(stderr, "ERROR: Invalid number of frames on command line. Must "
                      "be at least 2.\n");
      return 1;
    }
    long double xcenter = atof(argv[3]);
    long double ycenter = atof(argv[4]);
    long double zoom = atof(argv[5]);

    int framespersecond = 30;
    if (argc > 6)
    {
      framespersecond = atoi(argv[6]);
    }
    if (zoom < 1.0)
    {
      fprintf(stderr, "ERROR: zoom factor must be 1.0 or greater.\n");
      return 1;
    }

    std::vector<MidiNote> midiNotes = {};
    std::vector<MidiNote> midiPercussionNotes = {};
    int trackNumberCounter = 0;
    int percussionTrackNumberCounter = 0;
    MidiTrack inputFiles[1] = {
        // MidiTrack{.filename = "./input/percTest1PianoTrack.mid", .isPercussion = false},
        MidiTrack{.filename = "./input/discosnailschords.mid", .isPercussion = false},
        // MidiTrack{.filename = "./input/chordprogwurli.mid", .isPercussion = false},
    };

    MidiTrack percussionMidi = MidiTrack{.filename = "./input/discosnailsbetterbeat.mid", .isPercussion = true};

    // Percussion
    Options percussionOptions;
    std::vector<std::string> percussionsArguments = {"run", percussionMidi.filename};
    std::vector<char *> percussionFakeargv;
    for (const auto &arg : percussionsArguments)
      percussionFakeargv.push_back((char *)arg.data());
    percussionFakeargv.push_back(nullptr);
    std::cout << "Pushing mid file: " << percussionFakeargv[0] << "\n";
    percussionOptions.process(percussionFakeargv.size() - 1, percussionFakeargv.data(), 2);
    if (percussionOptions.getArgCount() != 1)
    {
      std::cerr << "At least one MIDI filename is required.\n";
      exit(1);
    }
    MidiFile percussionMidifile;
    percussionMidifile.read(percussionOptions.getArg(1));
    if (!percussionMidifile.status())
    {
      std::cerr << "Error reading MIDI file " << percussionOptions.getArg(1) << std::endl;
      exit(1);
    }
    percussionMidifile.joinTracks();
    percussionMidifile.doTimeAnalysis();
    percussionMidifile.linkNotePairs();

    // Percussion
    int percussiontrack = 0;
    for (int i = 0; i < percussionMidifile[percussiontrack].size(); i++)
    {
      if (!percussionMidifile[percussiontrack][i].isNoteOn())
      {
        continue;
      }
      std::cout << "Note start: " << percussionMidifile[percussiontrack][i].seconds
                << "; duration: " << percussionMidifile[percussiontrack][i].getDurationInSeconds()
                << "; P1 (pitch?): " << percussionMidifile[percussiontrack][i].getP1()
                << "; P2 (pitch?): " << percussionMidifile[percussiontrack][i].getP2()
                << "; P3 (pitch?): " << percussionMidifile[percussiontrack][i].getP3() << '\t'
                << "percussionTrack identifier?: " << percussionMidifile[percussiontrack][i][1] << std::endl;
      MidiNote newPercussionNote;

      newPercussionNote.pitch = percussionMidifile[percussiontrack][i].getP1();
      newPercussionNote.volume = percussionMidifile[percussiontrack][i].getP2();
      newPercussionNote.startSeconds = percussionMidifile[percussiontrack][i].seconds;
      if (percussionMidi.isPercussion)
      {
        newPercussionNote.endSeconds = percussionMidifile[percussiontrack][i].seconds +
                                       std::max(percussionMidifile[percussiontrack][i].getDurationInSeconds(), (double)(5 / framespersecond));
      }
      else
      {
        newPercussionNote.endSeconds = percussionMidifile[percussiontrack][i].seconds +
                                       // Tweak this if you want really snappy attack
                                       std::max(percussionMidifile[percussiontrack][i].getDurationInSeconds(), (double)(0.15)); // or 17 / framespersecond for snappy attacks
      }
      newPercussionNote.trackNumber = percussionTrackNumberCounter;
      newPercussionNote.isPercussion = percussionMidi.isPercussion;
      midiPercussionNotes.push_back(newPercussionNote);

      for (MidiTrack inputFile : inputFiles)
      {
        Options options;
        std::vector<std::string> arguments = {"run", inputFile.filename};
        std::vector<char *> fakeargv;
        for (const auto &arg : arguments)
          fakeargv.push_back((char *)arg.data());
        fakeargv.push_back(nullptr);
        std::cout << "Pushing mid file: " << fakeargv[0] << "\n";
        options.process(fakeargv.size() - 1, fakeargv.data(), 2);
        if (options.getArgCount() != 1)
        {
          std::cerr << "At least one MIDI filename is required.\n";
          exit(1);
        }
        MidiFile midifile;
        midifile.read(options.getArg(1));
        if (!midifile.status())
        {
          std::cerr << "Error reading MIDI file " << options.getArg(1) << std::endl;
          exit(1);
        }
        midifile.joinTracks();
        midifile.doTimeAnalysis();
        midifile.linkNotePairs();

        int track = 0;
        for (int i = 0; i < midifile[track].size(); i++)
        {
          if (!midifile[track][i].isNoteOn())
          {
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
          if (inputFile.isPercussion)
          {
            newNote.endSeconds = midifile[track][i].seconds +
                                 std::max(midifile[track][i].getDurationInSeconds(), (double)(5 / framespersecond));
          }
          else
          {
            newNote.endSeconds = midifile[track][i].seconds +
                                 // Tweak this if you want really snappy attack
                                 std::max(midifile[track][i].getDurationInSeconds(), (double)(0.15)); // or 17 / framespersecond for snappy attacks
          }
          newNote.trackNumber = trackNumberCounter;
          newNote.isPercussion = inputFile.isPercussion;
          midiNotes.push_back(newNote);
        }
        trackNumberCounter++;
      }
    }

    // std::vector<AubioNote> demoAudioNotes =
    //     ParseAubioNoteFile("./output/demoAudio.txt", 0.0);
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
    std::vector<float> percussionOnsets = {};
    // ParseOnsetSecondsFile("./output/rhythmInstrument1Onsets.txt");

    // return GenerateRippleZoomFrames(outdir, numframes, xcenter, ycenter, zoom,
    //                                 framespersecond, percussionOnsets,
    //                                 pitchedNotesVec, midiNotes);
    return GenerateMandleZoomFrames(outdir, numframes, xcenter, ycenter, zoom,
                                    framespersecond, percussionOnsets,
                                    pitchedNotesVec, midiNotes, midiPercussionNotes);
  }
  return PrintUsage();
}

static int PrintUsage()
{
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

static double GetTimestampSeconds(int framenumber, int framespersecond)
{
  return ((double)framenumber) / ((double)framespersecond);
}

static int GenerateRippleZoomFrames(
    const char *outdir, int numframes, long double xcenter, long double ycenter,
    long double zoom, int framespersecond, std::vector<float> onsetTimestamps,
    std::vector<std::vector<AubioNote>> aubioNotesVec,
    std::vector<MidiNote> midiNotes)
{

  std::vector<int> aubioMaxPitches{};
  std::vector<int> aubioMinPitches{};
  std::vector<int> aubioPitchRanges{};
  for (unsigned int i = 0; i < aubioNotesVec.size(); i++)
  {
    aubioMaxPitches.push_back(getMaxPitch(aubioNotesVec[i]));
    aubioMinPitches.push_back(getMinPitch(aubioNotesVec[i]));
    aubioPitchRanges.push_back((aubioMaxPitches[i] - aubioMinPitches[i]));
  }

  std::vector<PixelColor> availableColors = getColors();
  // Create a video frame buffer with 720p resolution (1280x720).
  // Dark mode
  PixelColor blankColorBlack;
  blankColorBlack.red = 0;
  blankColorBlack.green = 0;
  blankColorBlack.blue = 0;
  blankColorBlack.alpha = 255;
  PixelColor backgroundColor;
  backgroundColor.red = 0;
  backgroundColor.green = 0;
  backgroundColor.blue = 0;
  backgroundColor.alpha = 255;
  // Light mode
  // PixelColor blankColorWhite;
  // blankColorWhite.red = 205;
  // blankColorWhite.green = 203;
  // blankColorWhite.blue = 206;
  // blankColorWhite.alpha = 255;
  // PixelColor backgroundColor;
  // backgroundColor.red = 205;
  // backgroundColor.green = 203;
  // backgroundColor.blue = 206;
  // backgroundColor.alpha = 255;
  // Dark mode
  const int backgroundColorMaxSaturation = 20;
  const double onsetColorChangeLength = 0.4;

  const int scrollSpeedX = 0;
  const int scrollSpeedY = 0;

  // const double spinSpeedRadiansPerFrame = 0.01;
  // const double zoomMultiplierPerFrame = 1.01;

  VideoFrame currentFrame(xResolution, yResolution);
  currentFrame.SetAllPixels(backgroundColor);

  const int startTimeSeconds = 0;
  // Generate the frames
  for (int f = startTimeSeconds * framespersecond; f < numframes; ++f)
  {
    bool hasNewRipple = false;
    double timestamp = GetTimestampSeconds(f, framespersecond);

    // std::cout << " current timestamp " << timestamp << "\n  ";

    int onsetsPassed = 1;
    double lastOnsetTimestamp = -2 * onsetColorChangeLength;
    for (double onsetTimestamp : onsetTimestamps)
    {
      if (timestamp > onsetTimestamp)
      {
        onsetsPassed++;
        lastOnsetTimestamp = onsetTimestamp;
      }
    }

    // TODO: Fix this doing color jumps
    // backgroundColor = getNextBackgroundColor(
    //     timestamp, lastOnsetTimestamp, onsetColorChangeLength,
    //     onsetsPassed, backgroundColor, backgroundColorMaxSaturation);

    // currentFrame.ScrollPixels(scrollSpeedX, scrollSpeedY, blankColor);
    // currentFrame.SpinZoomPixels(spinSpeedRadiansPerFrame,
    //                             zoomMultiplierPerFrame, blankColor);

    std::vector<Ripple> ripples = {};
    for (unsigned int i = 0; i < aubioNotesVec.size(); ++i)
    {
      std::vector<AubioNote> notes = aubioNotesVec[i];
      AubioNote currentNote = getCurrentNote(notes, timestamp);
      if (currentNote.startSeconds != -1)
      {

        // std::cout << " setting new aubio ripple at " << timestamp << "\n  ";
        Ripple newRipple = getNoteRippleCircleOfScales(
            xResolution, yResolution, currentNote.pitch,
            currentNote.startSeconds, currentNote.endSeconds, framespersecond,
            i, false);
        hasNewRipple = true;

        // Ripple newRipple = getNoteRippleSidescrolling(
        //     0, xResolution - 1, 0, yResolution - 1, currentNote, minPitches,
        //     pitchRanges, framespersecond, i);
        ripples.push_back(newRipple);
      }
    }

    for (unsigned int i = 0; i < midiNotes.size(); ++i)
    {
      MidiNote checkNote = midiNotes[i];
      // Never check this note again if we're already past it's end time
      if (checkNote.endSeconds < timestamp)
      {
        midiNotes.erase(midiNotes.begin() + i);
        continue;
      }
      if (checkNote.startSeconds <= timestamp &&
          checkNote.endSeconds >= timestamp)
      {

        std::cout << " setting new midi ripple at " << timestamp << "\n  ";
        Ripple newRipple = getNoteRippleCircleOfScales(
            xResolution, yResolution, checkNote.pitch, checkNote.startSeconds,
            checkNote.endSeconds, framespersecond, checkNote.trackNumber, checkNote.isPercussion);
        hasNewRipple = true;

        // Ripple newRipple = getNoteRippleSidescrolling(
        //     0, xResolution - 1, 0, yResolution - 1, currentNote, minPitches,
        //     pitchRanges, framespersecond, i);
        ripples.push_back(newRipple);
      }
    }

    // Another thing to help with attack speed
    // const float rippleAttackSpeed = 1;
    const float rippleAttackSpeed = 1.8;

    // Dark mode
    // Medium darkening
    // currentFrame.BrightenAllPixels(0.9);
    // Rapid darkening, suitable for quick attack and decay instruments
    // currentFrame.BrightenAllPixels(0.78);

    // Dissolve the colors
    if (f % 9 == 0)
    // if (hasNewRipple)
    {
      for (int x = 0; x < xResolution / squareSize; ++x)
      {
        for (int y = 0; y < yResolution / squareSize; ++y)
        {
          const PixelColor middlePixel = currentFrame.GetPixel(x * squareSize + 4, y * squareSize + 4);
          int smallestSum = 257 * 3;
          bool hasTie = false;
          bool hasMultipleTies = false;
          int xDirection = -1;
          int yDirection = -1;
          int xTieDirection = -1;
          int yTieDirection = -1;

          for (int i = 0; i < 3; ++i)
          {
            for (int j = 0; j < 3; ++j)
            {
              if (i == 1 && j == 1)
              {
                continue;
              }

              const PixelColor pixel = currentFrame.GetPixel(x * squareSize + 3 + 1, y * squareSize + 3 + j);
              int squareSum = pixel.red + pixel.green + pixel.blue;
              if (squareSum < smallestSum)
              {
                smallestSum = squareSum;
                hasTie = false;
                hasMultipleTies = false;
                xDirection = i;
                yDirection = j;
              }
              else if (squareSum == smallestSum)
              {
                if (hasTie)
                {
                  hasMultipleTies = true;
                }
                else
                {
                  hasTie = true;
                  xTieDirection = i;
                  yTieDirection = j;
                }
              }
            }
          }

          if (hasMultipleTies)
          {
            xDirection = (int)((rand() % 4) - 2);
            yDirection = (int)((rand() % 4) - 2);
            // std::cout << "multitie x direction" << xDirection << "multitie y direction" << yDirection << "\n";
          }
          else
          {
            if (hasTie && (rand() % 2 == 0))
            {
              xDirection = xTieDirection;
              yDirection = yTieDirection;
              // std::cout << "tie x direction" << xDirection << "tie y direction" << yDirection << "\n";
            }
            else
            {
              // std::cout << "normal x direction" << xDirection << "normal y direction" << yDirection << "\n";
            }
          }

          for (int i = 0; i < squareSize; ++i)
          {
            for (int j = 0; j < squareSize; ++j)
            {
              int targetX = ((x + xDirection) * squareSize + i) % xResolution;
              int targetY = ((y + yDirection) * squareSize + j) % yResolution;
              if (targetX < 0)
              {
                targetX = xResolution + targetX;
              }
              if (targetY < 0)
              {
                targetY = yResolution + targetY;
              }
              const PixelColor targetPixel = currentFrame.GetPixel(targetX, targetY);
              currentFrame.CombinePixel(
                  x * squareSize + i, y * squareSize + j, 0.5,
                  targetPixel, 255, false);
              currentFrame.CombinePixel(
                  targetX, targetY, 0.5,
                  currentFrame.GetPixel(x * squareSize + i, y * squareSize + j),
                  255, false);
            }
          }
        }
      }
    }

    // Light mode
    // currentFrame.BrightenAllPixels(1.14);
    // TODO slow this down nicer
    // currentFrame.LinearStepAllPixelsTo(backgroundColor, 0.85);

    for (Ripple ripple : ripples)
    {
      int framesSinceRippleStart = f - ripple.startFrame;
      int outerRadius = std::min((framesSinceRippleStart * ripple.speed + 12) * rippleAttackSpeed, 30.0);
      // Make notes throb irregularly
      int innerRadius = outerRadius - (int)((5 + (ripple.startFrame + ripple.xCenter + ripple.yCenter) % 3) * std::sin((((ripple.startFrame + ripple.xCenter + ripple.yCenter) % 5 + 1) * framesSinceRippleStart * 0.01 * rippleAttackSpeed)));
      // int thickness =
      //     (ripple.thickness + (innerRadius / 2)) * ripple.thickness + (innerRadius / 2);
      // TODO: Extract discrete zoom and use it to move ripple centre as
      // well
      // For spreading effect
      // Coordinate zoomDiff = getDiscreteZoomDiff(
      //     ripple.xCenter, ripple.yCenter, xResolution / 2, yResolution / 2);

      int scrolledXCenter =
          ripple.xCenter
          // - framesSinceRippleStart * (scrollSpeedX + zoomDiff.realPart)
          ;
      int scrolledYCenter =
          ripple.yCenter
          // - framesSinceRippleStart * (scrollSpeedY + zoomDiff.imaginaryPart)
          ;

      // Decay multiplier - choose a lower number for plucky instruments, higher number / 1 for sustained instruments
      const float decayBrightnessMultiplier = std::max(std::pow(0.96, (double)std::max(framesSinceRippleStart - 15, 0)), 0.1);

      for (int x = 0; x < xResolution; ++x)
      {
        for (int y = 0; y < yResolution; ++y)
        {
          // const int distFromCentreSquared =
          //     (x - scrolledXCenter) * (x - scrolledXCenter) +
          //     (y - scrolledYCenter) * (y - scrolledYCenter);
          // TODO make the ripples more fun - maybe remove abs, maybe multiply
          // by another function of x and y
          // const int distFromCentreSquaredWonky =
          //     std::abs(std::pow((x - scrolledXCenter), ripple.type + 1) *
          //              (std::cos(ripple.type * 3 + 3 / 4) + 0.05)) +
          //     std::abs(std::pow((y - scrolledYCenter), ripple.type + 1) *
          //              (std::sin(ripple.type * 3 + 3 / 4) + 0.05));
          const int distFromCentreSquared = std::pow(x - scrolledXCenter, 2) +
                                            std::pow(y - scrolledYCenter, 2);
          if (
              // For a ringning effect
              // distFromCentreSquared > innerRadius * innerRadius - thickness &&
              distFromCentreSquared < innerRadius * innerRadius
              //  && framesSinceRippleStart % 5 == 0 - if you want a strobe party effect
          )
          {
            const float decayMultiplier = 0.9;
            // Dark mode
            currentFrame.AddPixel(x, y, ripple.addColor, decayBrightnessMultiplier);
            // Light mode
            // std::cout << "adding pixel"  << ripple.addColor.red << ripple.addColor.green << ripple.addColor.blue << "\n";
            // currentFrame.CombinePixel(x, y, 0.5, ripple.addColor, 255);
          }

          // Add a sportlight arc to the top corner
          // if ((std::abs(std::atan2(y, x) - std::atan2(scrolledYCenter, scrolledXCenter)) < 0.1) && ((x > 0) == (scrolledXCenter > 0)) && ((y > 0) == (scrolledYCenter > 0)))

          int originX = xResolution / 2;
          int originY = yResolution / 2;
          // Add an arc to the center
          float angleToCheckPoint = std::atan2(y - originY, x - originX);
          float angleToRipple = std::atan2(scrolledYCenter - originY, scrolledXCenter - originX);
          const int rippleDistFromOriginSquared = std::pow(std::abs(scrolledXCenter - originX), 2) +
                                                  std::pow(std::abs(scrolledYCenter - originY), 2);
          const int pointDistFromOriginSquared = std::pow(std::abs(x - originX) + outerRadius, 2) +
                                                 std::pow(std::abs(y - originY) + outerRadius, 2);
          float arcAngle = 0.15;
          if (pointDistFromOriginSquared >= (rippleDistFromOriginSquared))
          {
            arcAngle = 0.00;
          }
          else
          {
            arcAngle = arcAngle * std::pow(std::max(std::min(0.5 * rippleDistFromOriginSquared - std::abs(pointDistFromOriginSquared - 0.5 * rippleDistFromOriginSquared), (double)rippleDistFromOriginSquared), 0.1) / rippleDistFromOriginSquared, 2);
          }

          if (std::abs(angleToCheckPoint - angleToRipple) < arcAngle || std::abs(angleToCheckPoint - angleToRipple - 2 * M_PI) < arcAngle || std::abs(angleToCheckPoint - angleToRipple + 2 * M_PI) < arcAngle)
          {
            // Dark mode
            currentFrame.AddPixel(x, y, ripple.addColor, decayBrightnessMultiplier);
            // Light mode
            // currentFrame.CombinePixel(x, y, 0.5, ripple.addColor, 255);
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

static AubioNote getCurrentNote(std::vector<AubioNote> notes, float timestamp)
{
  bool noteIsPlaying = false;
  // Check changes from pitches
  for (unsigned int i = 0; i < notes.size(); i++)
  {
    AubioNote checkNote = notes[i];
    if (checkNote.startSeconds < timestamp &&
        checkNote.endSeconds > timestamp)
    {
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
                                    std::vector<std::vector<AubioNote>> aubioNotesVec,
                                    std::vector<MidiNote> midiNotes, std::vector<MidiNote> midiPercussionNotes)
{
  std::vector<PixelColor> availableColors = getColors();
  bool reverseDeadEnd = false;
  int framesSinceDeadEnd = 0;
  int framesSinceLastOnsetPassed = 0;
  const int framesToMoveCentres = framespersecond * 2;
  const bool onsetsModeMidi = true; // switch to false if using onsetTimestamps generated by aubio
  std::vector<int> aubioMaxPitches{};
  std::vector<int> aubioMinPitches{};
  std::vector<int> aubioPitchRanges{};
  std::vector<int> aubioMedianPitches{};
  for (unsigned int i = 0; i < aubioNotesVec.size(); i++)
  {
    aubioMaxPitches.push_back(getMaxPitch(aubioNotesVec[i]));
    aubioMinPitches.push_back(getMinPitch(aubioNotesVec[i]));
    aubioPitchRanges.push_back((aubioMaxPitches[i] - aubioMinPitches[i]));
    aubioMedianPitches.push_back((aubioMaxPitches[i] + aubioMinPitches[i]) / 2);
  }
  int midiMidianPitch = getMaxPitch(midiNotes) - getMinPitch(midiNotes);
  bool isSilent = true;
  // Create a video frame buffer with 720p resolution (1280x720).
  PixelColor blankColor;
  blankColor.red = 0;
  blankColor.green = 0;
  blankColor.blue = 0;
  blankColor.alpha = 255;
  VideoFrame currentFrame(xResolution, yResolution);
  for (unsigned int x = 0; x < xResolution; x++)
  {
    for (unsigned int y = 0; y < yResolution; y++)
    {
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
  float defaultPitch = aubioMedianPitches.size() > 0 ? aubioMedianPitches[0] : midiMidianPitch;
  float previousPitch = defaultPitch;
  float currentPitch = defaultPitch;
  float currentNoteLength = 0;
  long double pitchMultiplier = 0.008 * currentPitch / defaultPitch;
  long double targetPitchMultiplier = pitchMultiplier;
  std::cout << " initial pitch multiplier " << pitchMultiplier << "\n  ";
  std::cout << " initial target pitch multiplier " << targetPitchMultiplier
            << "\n  ";
  long double denom = 48.0; // Initial zoom
  Coordinate nextCentre = {};
  nextCentre.realPart = xcenter;
  nextCentre.imaginaryPart = ycenter;
  int framesSinceChangeOfCentre = 0;
  std::set<int> uniqueMandleCounts;
  int lastOnsetsPassed = 0;

  int mandleCounts[xResolution / squareSize][yResolution / squareSize];
  const int startTimeSeconds = 0;
  // Generate the frames
  for (int f = startTimeSeconds * framespersecond; f < numframes; ++f)
  {
    framesSinceDeadEnd++;
    framesSinceChangeOfCentre++;
    if (limit < maxLimit)
    {
      limit = limit * 1.012;
    }
    double timestamp = GetTimestampSeconds(f, framespersecond);

    // std::cout << " current timestamp " << timestamp << "\n  ";

    long double ver_span = 4.0 / denom;
    long double hor_span = ver_span * ((xResolution / squareSize) - 1.0) / ((yResolution / squareSize) - 1.0);
    long double ci_top = ycenter + ver_span / 2.0;
    long double yStepDistance = ver_span / ((yResolution / squareSize) - 1.0);
    long double cr_left = xcenter - hor_span / 2.0;
    long double xStepDistance = hor_span / ((xResolution / squareSize) - 1.0);

    if (framesSinceChangeOfCentre <= framesToMoveCentres)
    {
      xcenter = xcenter + ((nextCentre.realPart - xcenter) *
                           framesSinceChangeOfCentre / framesToMoveCentres);
      ycenter = ycenter + ((nextCentre.imaginaryPart - ycenter) *
                           framesSinceChangeOfCentre / framesToMoveCentres);
    }

    int onsetsPassed = 1;
    if (onsetsModeMidi)
    {
      for (MidiNote percNote : midiPercussionNotes)
      {
        if (timestamp > percNote.startSeconds)
        {
          onsetsPassed++;
        }
      }
    }
    else
    {

      for (double onsetTimestamp : onsetTimestamps)
      {
        if (timestamp > onsetTimestamp)
        {
          onsetsPassed++;
        }
      }
    }
    if (onsetsPassed != lastOnsetsPassed)
    {
      lastOnsetsPassed = onsetsPassed;
      framesSinceLastOnsetPassed = 0;
    }
    else
    {
      framesSinceLastOnsetPassed++;
    }

    targetPitchMultiplier = 0.001 * currentPitch / defaultPitch;
    pitchMultiplier =
        pitchMultiplier + (targetPitchMultiplier - pitchMultiplier) / 8;

    uniqueMandleCounts.clear();
    if (isSilent)
    {
      alphaModifier = alphaModifier - 0.025;
    }
    else
    {
      alphaModifier = alphaModifier - ((framesSinceChangeOfCentre - 1) * 0.025);
      if (currentNoteLength > 0)
      {
        alphaModifier =
            (framesSinceChangeOfCentre / framespersecond) / currentNoteLength;
      }
    }
    if (alphaModifier < 0)
    {
      alphaModifier = 0;
    }
    for (int x = 0; x < (xResolution / squareSize); ++x)
    {
      long double cr = getXPosition(x, cr_left, xStepDistance);
      for (int y = 0; y < (yResolution / squareSize); ++y)
      {
        long double ci = getYPosition(y, ci_top, yStepDistance);
        int count = Mandelbrot(cr, ci, limit);
        mandleCounts[x][y] = count;
        uniqueMandleCounts.insert(count);
        PixelColor color = Palette(
            count, limit, onsetsPassed, currentPitch, previousPitch,
            framesSinceChangeOfCentre, framesSinceLastOnsetPassed,
            alphaModifier, availableColors, currentFrame.GetPixel(x, y));
        for (int i = 0; i < squareSize; i++)
        {
          for (int j = 0; j < squareSize; j++)
          {
            currentFrame.SetPixel(x * squareSize + i, y * squareSize + j, color);
          }
        }
      }
    }
    if (uniqueMandleCounts.size() <= 4 &&
        framesSinceDeadEnd > framespersecond)
    {
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

    long double accelerationMultiplier = 0.000005 * (f);

    // Increase the zoom magnification for the next frame.
    long double multiplier =
        ((1 + pitchMultiplier + accelerationMultiplier) * smoothMultiplier);
    if (reverseDeadEnd)
    {
      if (multiplier < 2 && multiplier > 0)
      {
        multiplier = 2 - multiplier; // This is too much
      }
      else
      {
        multiplier = 1 / multiplier; // This is too much
      }
    }
    denom = denom * multiplier;

    bool noteIsPlaying = false;
    // Check changes from aubio pitches
    for (unsigned int i = 0; i < aubioNotesVec.size(); i++)
    {
      for (unsigned int j = 0; j < aubioNotesVec[i].size(); j++)
      {
        AubioNote checkNote = aubioNotesVec[i][j];
        if (checkNote.startSeconds < timestamp &&
            checkNote.endSeconds > timestamp)
        {
          noteIsPlaying = true;

          if (isSilent || checkNote.pitch != currentPitch)
          {
            framesSinceChangeOfCentre = 0;
            std::cout << "New aubio note pitch: ";
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
            int minXIndex = 1 + (xResolution / squareSize) / 10;
            int maxXIndex = (xResolution / squareSize) - 1 - (xResolution / squareSize) / 10;
            int minYIndex = 1 + (yResolution / squareSize) / 10;
            int maxYIndex = (yResolution / squareSize) - 1 - (yResolution / squareSize) / 10;
            std::vector<PixelIndex> interestingPoints =
                getInterestingPixelIndexes(mandleCounts, minXIndex, maxXIndex,
                                           minYIndex, maxYIndex);
            if (interestingPoints.size() > 0)
            {

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
            }
            else
            {
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
    }

    // Check changes from aubio pitches
    for (unsigned int i = 0; i < midiNotes.size(); i++)
    {
      MidiNote checkNote = midiNotes[i];
      if (checkNote.startSeconds > timestamp || checkNote.endSeconds < timestamp)
      {
        break;
      }
      noteIsPlaying = true;

      if (isSilent || checkNote.pitch != currentPitch)
      {
        framesSinceChangeOfCentre = 0;
        std::cout << "New midi note pitch at timestamp: " << timestamp;
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
        int minXIndex = 1 + (xResolution / squareSize) / 10;
        int maxXIndex = (xResolution / squareSize) - 1 - (xResolution / squareSize) / 10;
        int minYIndex = 1 + (yResolution / squareSize) / 10;
        int maxYIndex = (yResolution / squareSize) - 1 - (yResolution / squareSize) / 10;
        std::vector<PixelIndex> interestingPoints =
            getInterestingPixelIndexes(mandleCounts, minXIndex, maxXIndex,
                                       minYIndex, maxYIndex);
        if (interestingPoints.size() > 0)
        {

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
        }
        else
        {
          std::cout << nextCentre.realPart << "\n  ";
          std::cout << nextCentre.imaginaryPart << " \n  ";
          std::cout << xcenter << " - current real part  \n  ";
          std::cout << ycenter << " - current imaginary part \n  ";
          std::cout << "!!! NO INTERESTING POINTS, RANDOM CHOICE NOT GONNA "
                       "WORK !!!";
        }
      }
    }
    if (noteIsPlaying == false)
    {
      isSilent = true;
      if (currentPitch != defaultPitch)
      {
        previousPitch = currentPitch;
        currentPitch = defaultPitch;
        currentNoteLength = 0;
      }
    }
    if (framesSinceChangeOfCentre > 1)
    {
      int minXIndex = 1;
      int maxXIndex = xResolution - 1;
      int minYIndex = 1;
      int maxYIndex = yResolution - 1;
      std::vector<PixelIndex> interestingPoints = getInterestingPixelIndexes(
          mandleCounts, minXIndex, maxXIndex, minYIndex, maxYIndex);
      // get interesting point from near target centre if pitch is unchanged
      // (so we constantly add precision as we zoom)
      if (interestingPoints.size() > 0)
      {
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
      }
      else
      {
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
