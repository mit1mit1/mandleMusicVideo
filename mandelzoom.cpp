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
#include <cmath>
#include <vector>
#include "lodepng.h"
#include <algorithm>
#include <fstream>

struct PixelColor
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
};

struct Coordinate
{
    double realPart;
    double imaginaryPart;
};

struct AubioNote
{
    float pitch;
    float startSeconds;
    float endSeconds;
};

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
static int Mandelbrot(double cr, double ci, int limit);
static PixelColor Palette(int count, int limit, int onsetsPassed);

int main(int argc, const char *argv[])
{
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
        // Create a video frame buffer with 720p resolution (1280x720).
        const int width = 1280;
        const int height = 720;
        VideoFrame frame(width, height);

        const int limit = 16000;
        // Below provides a smooth zoom all the way to the specified max zoom
        // double multiplier = pow(zoom, 1.0 / (numframes - 1.0));

        double multiplier = pow(zoom, 1.0 / (100 - 1.0));
        double denom = 1.0;
        float currentPitch = 44;
        Coordinate nextCentre = {};
        nextCentre.realPart = xcenter;
        nextCentre.imaginaryPart = ycenter;

        for (int f = 0; f < numframes; ++f)
        {
            double timestamp = GetTimestampSeconds(f, framespersecond);
            // Calculate the real and imaginary range of values for each frame.
            // The zoom is exponential.
            // On the first frame, the scale is such that the smaller dimension (height) spans 4 units
            // from the bottom of the frame to the top.
            // On the last frame, the scale is that number of units divided by 'zoom'.
            double ver_span = 4.0 / denom;
            double hor_span = ver_span * (width - 1.0) / (height - 1.0);
            double ci_top = ycenter + ver_span / 2.0;
            double ci_delta = ver_span / (height - 1.0);
            double cr_left = xcenter - hor_span / 2.0;
            double cr_delta = hor_span / (width - 1.0);

            int onsetsPassed = 1;
            for (double onsetTimestamp : onsetTimestamps)
            {
                if (timestamp > onsetTimestamp)
                {
                    onsetsPassed++;
                }
            }

            auto IsInNote = [=](AubioNote note)
            {
                return timestamp > note.startSeconds and timestamp < note.endSeconds;
            };
            std::vector<AubioNote>::iterator currentNote = std::find_if(notes.begin(), notes.end(), IsInNote);
            currentPitch = ((*currentNote).pitch > 0) ? (*currentNote).pitch : 44;

            multiplier = pow(zoom, 1.0 / (currentPitch - 1.0));

            for (AubioNote note : notes)
            {
                if (note.startSeconds < timestamp && note.endSeconds > timestamp)
                {
                    if (note.pitch != currentPitch)
                    {
                        currentPitch = note.pitch;
                        nextCentre.realPart = 32;
                        nextCentre.imaginaryPart = 32;
                    }
                    break;
                }
            }

            for (int x = 0; x < width; ++x)
            {
                double cr = cr_left + x * cr_delta;
                for (int y = 0; y < height; ++y)
                {
                    double ci = ci_top - y * ci_delta;
                    int count = Mandelbrot(cr, ci, limit);
                    PixelColor color = Palette(count, limit, onsetsPassed);
                    frame.SetPixel(x, y, color);
                }
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

            // Increase the zoom magnification for the next frame.
            denom *= multiplier;
        }
        return 0;
    }
    catch (const char *message)
    {
        fprintf(stderr, "EXCEPTION: %s\n", message);
        return 1;
    }
}

static int Mandelbrot(double cr, double ci, int limit)
{
    int count = 0;
    double zr = 0.0;
    double zi = 0.0;
    double zr2 = 0.0;
    double zi2 = 0.0;
    while ((count < limit) && (zr2 + zi2 < 4.001))
    {
        double tzi = 2.0 * zr * zi + ci;
        zr = zr2 - zi2 + cr;
        zi = tzi;
        zr2 = zr * zr;
        zi2 = zi * zi;
        ++count;
    }
    return count;
}

static double ZigZag(double x)
{
    double y = fmod(fabs(x), 2.0);
    if (y > 1.0)
        y = 1.0 - y;
    return y;
}

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
