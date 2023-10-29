/*
    mandelzoom.cpp

    MIT License

    Copyright (c) 2019 Don Cross <cosinekitty@gmail.com>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
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

static std::vector<AubioNote> ParseAubioNoteFile(const char *filename);
static std::vector<float> ParseOnsetSecondsFile(const char *filename);
static int PrintUsage();
static int GenerateZoomFrames(const char *outdir, int numframes, double xcenter, double ycenter, double zoom, int framespersecond, std::vector<float> onsetTimestamps, std::vector<AubioNote> notes);
static double GetTimestampSeconds(int framenumber, int framespersecond);
static PixelColor Palette(int count, int limit, int onsetsPassed);

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
        double xcenter = atof(argv[3]);
        double ycenter = atof(argv[4]);
        double zoom = atof(argv[5]);

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
        std::vector<AubioNote> fingerNotes = ParseAubioNoteFile("/home/midly/apps/mandelMusicalZoom/fingerPickingNotes1.txt");
        std::vector<float> percussionOnsets = ParseOnsetSecondsFile("/home/midly/apps/mandelMusicalZoom/percussionOnset.txt");

        return GenerateZoomFrames(outdir, numframes, xcenter, ycenter, zoom, framespersecond, percussionOnsets, fingerNotes);
    }

    return PrintUsage();
}

static std::vector<AubioNote> ParseAubioNoteFile(const char *filename)
{
    std::ifstream infile(filename);
    std::cout << " Got the file ";

    float x1, x2, x3;

    std::vector<AubioNote> notes{};

    while (infile >> x1 >> x2 >> x3)
    {
        AubioNote lineNote;
        lineNote.pitch = x1;
        lineNote.startSeconds = x2;
        lineNote.endSeconds = x3;
        notes.push_back(lineNote);
    }

    std::cout << "First Note: ";
    std::cout << notes.front().startSeconds << " start sec  " << notes.front().endSeconds << " end sec  " << notes.front().pitch << " pitch ";

    std::cout << "Last Note: ";
    std::cout << notes.back().startSeconds << " start sec  " << notes.back().endSeconds << " end sec  " << notes.back().pitch << " pitch ";

    return notes;
}

// TODO zoom out during silence

// TODO for both onsets and notes keep track of total number of notes and get the average length between notes
static std::vector<float> ParseOnsetSecondsFile(const char *filename)
{
    std::ifstream infile(filename);
    std::cout << " Got the file ";

    float x1;

    std::vector<float> onsets{};

    while (infile >> x1)
    {
        onsets.push_back(x1);
    }

    std::cout << "First Onset: ";
    std::cout << onsets.front() << "  sec  ";

    std::cout << "Last Onset: ";
    std::cout << onsets.back() << " sec  ";

    return onsets;
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

static int GenerateZoomFrames(const char *outdir, int numframes, double xcenter, double ycenter, double zoom, int framespersecond, std::vector<float> onsetTimestamps, std::vector<AubioNote> notes)
{
    try
    {
        const int pitchSum = getPitchSum(notes);
        const double averagePitch = pitchSum / notes.size();
        std::cout << " average pitch " << pitchSum << "\n  " << notes.size() << "\n  " << averagePitch << "\n  ";
        const double silentPitch = averagePitch / 10 * (-1);
        std::cout << " silent pitch " << silentPitch;
        // Create a video frame buffer with 720p resolution (1280x720).
        VideoFrame frame(xResolution, yResolution);

        const int limit = 16000;
        // Below provides a smooth zoom all the way to the specified max zoom
        // double multiplier = pow(zoom, 1.0 / (numframes - 1.0));
        double smoothMultiplier = pow(zoom, 1.0 / (numframes - 1.0));

        float currentPitch = silentPitch;
        double pitchMultiplier = 0.04 * currentPitch / averagePitch;
        double targetPitchMultiplier = pitchMultiplier;
        std::cout << " initial pitch multiplier " << pitchMultiplier << "\n  ";
        std::cout << " initial target pitch multiplier " << targetPitchMultiplier << "\n  ";
        double denom = 1.0;
        Coordinate nextCentre = {};
        nextCentre.realPart = xcenter;
        nextCentre.imaginaryPart = ycenter;
        int framesSinceChangeOfCentre = 0;
        std::set<int> uniqueMandleCounts;

        int mandleCounts[xResolution][yResolution];
        int deadEndMultiplier = 1;

        for (int f = 0; f < numframes; ++f)
        {
            double timestamp = GetTimestampSeconds(f, framespersecond);

            std::cout << " current timestamp " << timestamp << "\n  ";
            // Calculate the real and imaginary range of values for each frame.
            // The zoom is exponential.
            // On the first frame, the scale is such that the smaller dimension (height) spans 4 units
            // from the bottom of the frame to the top.
            // On the last frame, the scale is that number of units divided by 'zoom'.
            double ver_span = 4.0 / denom;
            double hor_span = ver_span * (xResolution - 1.0) / (yResolution - 1.0);
            double ci_top = ycenter + ver_span / 2.0;
            double ci_delta = ver_span / (yResolution - 1.0);
            double cr_left = xcenter - hor_span / 2.0;
            double cr_delta = hor_span / (xResolution - 1.0);
            std::cout << " previous centre " << xcenter << ", " << ycenter << "\n  ";
            xcenter = xcenter + (nextCentre.realPart - xcenter) / 8;
            ycenter = ycenter + (nextCentre.realPart - ycenter) / 8;
            std::cout << " new centre " << xcenter << ", " << ycenter << "\n  ";

            int onsetsPassed = 1;
            for (double onsetTimestamp : onsetTimestamps)
            {
                if (timestamp > onsetTimestamp)
                {
                    onsetsPassed++;
                }
            }

            // auto IsInNote = [=](AubioNote note)
            // {
            //     return timestamp > note.startSeconds && timestamp < note.endSeconds;
            // };
            // std::vector<AubioNote>::iterator currentNote = std::find_if(notes.begin(), notes.end(), IsInNote);
            // currentPitch = ((*currentNote).pitch > 0) ? (*currentNote).pitch : 44;

            // std::cout << " previous pitch multiplier " << pitchMultiplier << "\n  ";
            // std::cout << " previous target pitch multiplier " << targetPitchMultiplier << "\n  ";
            targetPitchMultiplier = 0.04 * currentPitch / averagePitch;
            // std::cout << " new target pitch multiplier " << targetPitchMultiplier << "\n  ";
            pitchMultiplier = pitchMultiplier + (targetPitchMultiplier - pitchMultiplier) / 8;
            // std::cout << " new pitch multiplier " << pitchMultiplier << "\n  ";

            uniqueMandleCounts.clear();
            for (int x = 0; x < xResolution; ++x)
            {
                double cr = cr_left + x * cr_delta;
                for (int y = 0; y < yResolution; ++y)
                {
                    double ci = ci_top - y * ci_delta;
                    int count = Mandelbrot(cr, ci, limit);
                    // TODO: check unique counts and if not enough, add a zoom out multiplier.
                    mandleCounts[x][y] = count;
                    uniqueMandleCounts.insert(count);
                    PixelColor color = Palette(count, limit, onsetsPassed);
                    frame.SetPixel(x, y, color);
                }
            }
            if (uniqueMandleCounts.size() <= 1)
            {
                deadEndMultiplier = 0.8;
            }
            else
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

            printf("Wrote %s\n", filename.c_str());

            // std::cout << " previous denom " << denom << "\n  ";
            // Increase the zoom magnification for the next frame.
            denom = denom * (0.99 + pitchMultiplier) * smoothMultiplier * deadEndMultiplier;
            // std::cout << " new denom " << denom << "\n  ";

            bool noteIsPlaying = false;
            // Check changes from pitches
            for (unsigned int i = 0; i < notes.size(); i++)
            {
                AubioNote checkNote = notes[i];
                if (checkNote.startSeconds < timestamp && checkNote.endSeconds > timestamp)
                {
                    noteIsPlaying = true;

                    std::cout << "Note pitch]: ";
                    std::cout << checkNote.pitch << "\n  ";
                    std::cout << checkNote.startSeconds << "\n  ";
                    std::cout << checkNote.endSeconds << "\n  ";
                    if (checkNote.pitch != currentPitch)
                    {
                        framesSinceChangeOfCentre = 0;
                        std::cout << "Changed pitch so setting new centre";
                        currentPitch = checkNote.pitch;
                        // TODO set quadrant to get interesting point from based on what the pitch is
                        int minXIndex = (((int)currentPitch % 2) * xResolution / 2) + 1;
                        int maxXIndex = minXIndex + xResolution / 2 - 2;
                        int minYIndex = ((((int)currentPitch * 7) % 2) * yResolution / 2) + 1;
                        int maxYIndex = minYIndex + yResolution / 2 - 2;
                        std::vector<PixelIndex> interestingPoints = getInterestingPixelIndexes(mandleCounts, minXIndex, maxXIndex, minYIndex, maxYIndex);
                        if (interestingPoints.size() > 0)
                        {
                            Coordinate nextInterstingPoint = chooseRandomInterestingPoint(interestingPoints,
                                                                                    cr_delta, ci_delta, xcenter, ycenter);
                            nextCentre.realPart = nextInterstingPoint.realPart;
                            nextCentre.imaginaryPart = nextInterstingPoint.imaginaryPart;

                            std::cout << "Real part of next centre: ";
                            std::cout << nextCentre.realPart << "\n  ";

                            std::cout << "Imaginary part of next centre: ";
                            std::cout << nextCentre.imaginaryPart << " \n  ";
                        }
                    }
                    else
                    {
                        framesSinceChangeOfCentre = framesSinceChangeOfCentre + 1;
                    }
                    break;
                }
            }
            if (noteIsPlaying == false)
            {
                if (currentPitch == silentPitch)
                {
                    framesSinceChangeOfCentre = framesSinceChangeOfCentre + 1;
                }
                else
                {
                    currentPitch = silentPitch;
                }
            }
            if (framesSinceChangeOfCentre > 10)
            {
                std::cout << "No change pitch so setting new centre close to target centre \n";
                int minXIndex = xResolution * 4 / 10;
                int maxXIndex = xResolution * 6 / 10;
                int minYIndex = yResolution * 4 / 10;
                int maxYIndex = yResolution * 6 / 10;
                std::vector<PixelIndex> interestingPoints = getInterestingPixelIndexes(mandleCounts, minXIndex, maxXIndex, minYIndex, maxYIndex);
                if (interestingPoints.size() > 0)
                {
                    Coordinate nextInterstingPoint = chooseRandomInterestingPoint(interestingPoints,
                                                                            cr_delta, ci_delta, xcenter, ycenter);

                    nextCentre.realPart = nextInterstingPoint.realPart;
                    nextCentre.imaginaryPart = nextInterstingPoint.imaginaryPart;
                    std::cout << "Real part of next centre: ";
                    std::cout << nextCentre.realPart << "\n  ";
                    std::cout << "Imaginary part of next centre: ";
                    std::cout << nextCentre.imaginaryPart << " \n  ";
                }
                // TODO get interesting point from near current centre if pitch is unchanged (so we constantly add precision as we zoom)
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

static double ZigZag(double x)
{
    double y = fmod(fabs(x), 2.0);
    if (y > 1.0)
        y = 1.0 - y;
    return y;
}

// TODO more create colors - e.g. setting pallette to a length of 12
// setting the first three elements based on track 1's pitch,
// the next three based on track 2's pitch,
// the third three based on track 3's pitch,
// and the final 3 based on percussion onsets passed.
static PixelColor Palette(int count, int limit, int onsetsPassed)
{
    PixelColor color;
    if (count >= limit)
    {
        color.red = color.green = color.blue = 0;
    }
    else
    {
        double x = static_cast<double>(count) / (limit - 1.0);
        color.red = static_cast<unsigned char>(255.0 * ZigZag(0.5 + 7.0 * x + 0.13 * onsetsPassed));
        color.green = static_cast<unsigned char>(255.0 * ZigZag(0.2 + 9.0 * x + 0.15 * onsetsPassed));
        color.blue = static_cast<unsigned char>(255.0 * ZigZag(0.7 + 11.0 * x + 0.19 * onsetsPassed));
    }
    color.alpha = 255;

    return color;
}
