/*
    mandelzoom.cpp

    MIT License

    Copyright (c) 2019 Don Cross <cosinekitty@gmail.com>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <set>
#include "lodepng.h"
#include <algorithm>
#include <fstream>
#include "aubioParser.h"
#include "colors.h"
#include "math.h"

class VideoFrame
{
private:
    int width;
    int height;
    std::vector<unsigned char> buffer;

public:
    VideoFrame(int _width, int _height)
        : width(_width), height(_height), buffer(4 * _width * _height, 255)
    {
    }

    void SetPixel(int x, int y, PixelColor color)
    {
        int index = 4 * (y * width + x);
        buffer[index] = color.red;
        buffer[index + 1] = color.green;
        buffer[index + 2] = color.blue;
        buffer[index + 3] = color.alpha;
    }

    int SavePng(const char *outFileName)
    {
        unsigned error = lodepng::encode(outFileName, buffer, width, height);
        if (error)
        {
            fprintf(stderr, "ERROR: lodepng::encode returned %u\n", error);
            return 1;
        }
        return 0;
    }
};

static int PrintUsage();
static int GenerateZoomFrames(const char *outdir, int numframes, long double xcenter, long double ycenter, long double zoom, int framespersecond, std::vector<float> onsetTimestamps, std::vector<AubioNote> notes);
static double GetTimestampSeconds(int framenumber, int framespersecond);

int main(int argc, const char *argv[])
{
    std::cout << std::setprecision(13) << std::fixed;
    if (argc >= 6)
    {
        const char *outdir = argv[1];
        int numframes = atoi(argv[2]);
        if (numframes < 2)
        {
            fprintf(stderr, "ERROR: Invalid number of frames on command line. Must be at least 2.\n");
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
        std::vector<AubioNote> fingerNotes = ParseAubioNoteFile("./pitchedInstrument1Notes.txt");
        std::vector<float> percussionOnsets = ParseOnsetSecondsFile("./rhythmInstrument1Onsets.txt");

        return GenerateZoomFrames(outdir, numframes, xcenter, ycenter, zoom, framespersecond, percussionOnsets, fingerNotes);
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

static int GenerateZoomFrames(const char *outdir, int numframes, long double xcenter, long double ycenter, long double zoom, int framespersecond, std::vector<float> onsetTimestamps, std::vector<AubioNote> notes)
{
    try
    {
        std::vector<PixelColor> availableColors = getColors();
        bool deadEnd = false;
        int framesSinceDeadEnd = 0;
        const int framesToMoveCentres = 24;
        const int pitchSum = getPitchSum(notes);
        const double averagePitch = pitchSum / notes.size();
        std::cout << " average pitch " << pitchSum << "\n  " << notes.size() << "\n  " << averagePitch << "\n  ";
        const double defaultPitch = averagePitch / 30 * (-1);
        bool isSilent = true;
        std::cout << " silent pitch " << defaultPitch;
        // Create a video frame buffer with 720p resolution (1280x720).
        VideoFrame frame(xResolution, yResolution);

        // TODO: increase limit as we get deeper
        const int maxLimit = 16000;
        const int minLimit = 1600;
        int limit = minLimit;
        // Below provides a smooth zoom all the way to the specified max zoom
        // double multiplier = pow(zoom, 1.0 / (numframes - 1.0));
        long double smoothMultiplier = pow(zoom, 1.0 / (numframes - 1.0));
        float alphaModifier = 1.0;
        float previousPitch = defaultPitch;
        float currentPitch = defaultPitch;
        float currentNoteLength = 0;
        long double pitchMultiplier = 0.04 * currentPitch / averagePitch;
        long double targetPitchMultiplier = pitchMultiplier;
        std::cout << " initial pitch multiplier " << pitchMultiplier << "\n  ";
        std::cout << " initial target pitch multiplier " << targetPitchMultiplier << "\n  ";
        long double denom = 1.0;
        Coordinate nextCentre = {};
        nextCentre.realPart = xcenter;
        nextCentre.imaginaryPart = ycenter;
        int framesSinceChangeOfCentre = 0;
        std::set<int> uniqueMandleCounts;

        int mandleCounts[xResolution][yResolution];
        int deadEndMultiplier = 1;

        for (int f = 0; f < numframes; ++f)
        {
            framesSinceDeadEnd++;
            framesSinceChangeOfCentre++;
            if (limit < maxLimit)
            {
                limit = limit * 1.001;
            }
            double timestamp = GetTimestampSeconds(f, framespersecond);

            std::cout << " current timestamp " << timestamp << "\n  ";

            long double ver_span = 4.0 / denom;
            long double hor_span = ver_span * (xResolution - 1.0) / (yResolution - 1.0);
            long double ci_top = ycenter + ver_span / 2.0;
            long double yStepDistance = ver_span / (yResolution - 1.0);
            long double cr_left = xcenter - hor_span / 2.0;
            long double xStepDistance = hor_span / (xResolution - 1.0);

            if (framesSinceChangeOfCentre <= framesToMoveCentres)
            {
                xcenter = xcenter + ((nextCentre.realPart - xcenter) * framesSinceChangeOfCentre / framesToMoveCentres);
                ycenter = ycenter + ((nextCentre.imaginaryPart - ycenter) * framesSinceChangeOfCentre / framesToMoveCentres);
            }

            int onsetsPassed = 1;
            for (double onsetTimestamp : onsetTimestamps)
            {
                if (timestamp > onsetTimestamp)
                {
                    onsetsPassed++;
                }
            }

            targetPitchMultiplier = 0.04 * currentPitch / averagePitch;
            pitchMultiplier = pitchMultiplier + (targetPitchMultiplier - pitchMultiplier) / 8;

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
                    alphaModifier = (framesSinceChangeOfCentre / framespersecond) / currentNoteLength;
                }
            }
            if (alphaModifier < 0)
            {
                alphaModifier = 0;
            }
            for (int x = 0; x < xResolution; ++x)
            {
                long double cr = getXPosition(x, cr_left, xStepDistance);
                for (int y = 0; y < yResolution; ++y)
                {
                    long double ci = getYPosition(y, ci_top, yStepDistance);
                    int count = Mandelbrot(cr, ci, limit);
                    mandleCounts[x][y] = count;
                    uniqueMandleCounts.insert(count);
                    PixelColor color = Palette(count, limit, onsetsPassed, currentPitch, previousPitch, framesSinceChangeOfCentre, alphaModifier, availableColors);
                    frame.SetPixel(x, y, color);
                }
            }
            if (uniqueMandleCounts.size() <= 4)
            {
                framesSinceDeadEnd = 0;
                deadEndMultiplier = 0.9;
            }
            else if (framesSinceDeadEnd > 45)
            {
                deadEndMultiplier = 1.0;
            }

            // Create the output PNG filename in the format "outdir/frame_12345.png".
            char number[20];
            snprintf(number, sizeof(number), "%05d", f);
            std::string filename = std::string(outdir) + "/frame_" + number + ".png";

            // Save the video frame as a PNG file.
            int error = frame.SavePng(filename.c_str());
            if (error)
                return error;

            // Increase the zoom magnification for the next frame.
            if (deadEnd)
            {
                denom = denom / ((1 + pitchMultiplier) * smoothMultiplier * deadEndMultiplier);
            }
            else
            {
                denom = denom * ((1 + pitchMultiplier) * smoothMultiplier * deadEndMultiplier);
            }

            bool noteIsPlaying = false;
            // Check changes from pitches
            for (unsigned int i = 0; i < notes.size(); i++)
            {
                AubioNote checkNote = notes[i];
                if (checkNote.startSeconds < timestamp && checkNote.endSeconds > timestamp)
                {
                    noteIsPlaying = true;

                    if (isSilent || checkNote.pitch != currentPitch)
                    {
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
                        int minXIndex = 1;
                        int maxXIndex = xResolution - 1;
                        int minYIndex = 1;
                        int maxYIndex = yResolution - 1;
                        std::vector<PixelIndex> interestingPoints = getInterestingPixelIndexes(mandleCounts, minXIndex, maxXIndex, minYIndex, maxYIndex);
                        if (interestingPoints.size() > 0)
                        {

                            std::cout << nextCentre.realPart << " - next real part  \n  ";
                            std::cout << nextCentre.imaginaryPart << " - next imaginary part \n  ";
                            std::cout << xcenter << " - current real part  \n  ";
                            std::cout << ycenter << " - current imaginary part \n  ";
                            Coordinate nextInterstingPoint = chooseRandomInterestingPoint(interestingPoints,
                                                                                          xStepDistance, yStepDistance, xcenter, ycenter, cr_left, ci_top);
                            nextCentre.realPart = nextInterstingPoint.realPart;
                            nextCentre.imaginaryPart = nextInterstingPoint.imaginaryPart;
                        }
                        else
                        {
                            std::cout << nextCentre.realPart << "\n  ";
                            std::cout << nextCentre.imaginaryPart << " \n  ";
                            std::cout << xcenter << " - current real part  \n  ";
                            std::cout << ycenter << " - current imaginary part \n  ";
                            std::cout << "!!! NO INTERESTING POINTS, RANDOM CHOICE NOT GONNA WORK !!!";
                        }
                    }
                    break;
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
                std::vector<PixelIndex> interestingPoints = getInterestingPixelIndexes(mandleCounts, minXIndex, maxXIndex, minYIndex, maxYIndex);
                // get interesting point from near target centre if pitch is unchanged (so we constantly add precision as we zoom)
                if (interestingPoints.size() > 0)
                {
                    Coordinate nextInterstingPoint = chooseClosestInterestingPoint(
                        interestingPoints, xStepDistance, yStepDistance, xcenter, ycenter, nextCentre.realPart, nextCentre.imaginaryPart, cr_left, ci_top);

                    std::cout << "Focussing on next interesting point:  \n  ";
                    std::cout << nextCentre.realPart << " - next real part  \n  ";
                    std::cout << nextCentre.imaginaryPart << " - next imaginary part \n  ";
                    std::cout << xcenter << " - current real part  \n  ";
                    std::cout << ycenter << " - current imaginary part \n  ";
                    nextCentre.realPart = nextInterstingPoint.realPart;
                    nextCentre.imaginaryPart = nextInterstingPoint.imaginaryPart;
                    deadEnd = false;
                }
                else
                {
                    // TODO: Try lower and lower 'interesting' thresholds
                    std::cout << nextCentre.realPart << "\n  ";
                    std::cout << nextCentre.imaginaryPart << " \n  ";
                    std::cout << xcenter << " - current real part  \n  ";
                    std::cout << ycenter << " - current imaginary part \n  ";
                    std::cout << "!!! NO INTERESTING POINTS, FOCUS ON EXISTING POINT NOT GONNA WORK !!!";
                    deadEnd = true;
                    return 0;
                }
            }
        }
        return 0;
    }
    catch (const char *message)
    {
        fprintf(stderr, "EXCEPTION: %s\n", message);
        return 1;
    }
}
