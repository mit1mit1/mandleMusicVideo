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
static int GenerateZoomFrames(const char *outdir, int numframes, long double xcenter, long double ycenter, long double zoom, int framespersecond, std::vector<float> onsetTimestamps, std::vector<AubioNote> notes);
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

static int GenerateZoomFrames(const char *outdir, int numframes, long double xcenter, long double ycenter, long double zoom, int framespersecond, std::vector<float> onsetTimestamps, std::vector<AubioNote> notes)
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
        long double smoothMultiplier = pow(zoom, 1.0 / (numframes - 1.0));

        float currentPitch = silentPitch;
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
            double timestamp = GetTimestampSeconds(f, framespersecond);

            std::cout << " current timestamp " << timestamp << "\n  ";
            // Calculate the real and imaginary range of values for each frame.
            // The zoom is exponential.
            // On the first frame, the scale is such that the smaller dimension (height) spans 4 units
            // from the bottom of the frame to the top.
            // On the last frame, the scale is that number of units divided by 'zoom'.
            long double ver_span = 4.0 / denom;
            long double hor_span = ver_span * (xResolution - 1.0) / (yResolution - 1.0);
            long double ci_top = ycenter + ver_span / 2.0;
            long double ci_delta = ver_span / (yResolution - 1.0);
            long double cr_left = xcenter - hor_span / 2.0;
            long double cr_delta = hor_span / (xResolution - 1.0);
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
                long double cr = cr_left + x * cr_delta;
                for (int y = 0; y < yResolution; ++y)
                {
                    long double ci = ci_top - y * ci_delta;
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
            if (framesSinceChangeOfCentre > 0)
            {
                std::cout << "No change pitch so setting new centre close to target centre \n";
                int minXIndex = 1;
                int maxXIndex = xResolution - 1;
                int minYIndex = 1;
                int maxYIndex = yResolution - 1;
                std::vector<PixelIndex> interestingPoints = getInterestingPixelIndexes(mandleCounts, minXIndex, maxXIndex, minYIndex, maxYIndex);
                // get interesting point from near target centre if pitch is unchanged (so we constantly add precision as we zoom)
                if (interestingPoints.size() > 0)
                {
                    Coordinate nextInterstingPoint = chooseClosestInterestingPoint(
                        interestingPoints, cr_delta, ci_delta, xcenter, ycenter, nextCentre.realPart, nextCentre.imaginaryPart);

                    nextCentre.realPart = nextInterstingPoint.realPart;
                    nextCentre.imaginaryPart = nextInterstingPoint.imaginaryPart;
                    std::cout << "Real part of next centre: ";
                    std::cout << nextCentre.realPart << "\n  ";
                    std::cout << "Imaginary part of next centre: ";
                    std::cout << nextCentre.imaginaryPart << " \n  ";
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

// TODO more create colors - e.g. setting pallette to a length of 12
// setting the first three elements based on track 1's pitch,
// the next three based on track 2's pitch,
// the third three based on track 3's pitch,
// and the final 3 based on percussion onsets passed.

static PixelColor Palette(int count, int limit, int onsetsPassed)
{

    // Rolling in the deep colors

    // e8d6cb rgb(232, 214, 203)
    PixelColor col_e8d6cb;
    col_e8d6cb.red = static_cast<unsigned char>(232);
    col_e8d6cb.green = static_cast<unsigned char>(214);
    col_e8d6cb.blue = static_cast<unsigned char>(203);
    col_e8d6cb.alpha = static_cast<unsigned char>(255);
    // d0ada7 rgb(208, 173, 167)
    PixelColor col_d0ada7;
    col_d0ada7.red = static_cast<unsigned char>(208);
    col_d0ada7.green = static_cast<unsigned char>(173);
    col_d0ada7.blue = static_cast<unsigned char>(167);
    col_d0ada7.alpha = static_cast<unsigned char>(255);
    // ad6a6c rgb(173, 106, 108)
    PixelColor col_ad6a6c;
    col_ad6a6c.red = static_cast<unsigned char>(173);
    col_ad6a6c.green = static_cast<unsigned char>(106);
    col_ad6a6c.blue = static_cast<unsigned char>(108);
    col_ad6a6c.alpha = static_cast<unsigned char>(255);
    // 5d2e46 rgb(93, 46, 70)
    PixelColor col_5d2e46;
    col_5d2e46.red = static_cast<unsigned char>(93);
    col_5d2e46.green = static_cast<unsigned char>(46);
    col_5d2e46.blue = static_cast<unsigned char>(70);
    col_5d2e46.alpha = static_cast<unsigned char>(255);
    // b58db6 rgb(181, 141, 182)
    PixelColor col_b58db6;
    col_b58db6.red = static_cast<unsigned char>(181);
    col_b58db6.green = static_cast<unsigned char>(141);
    col_b58db6.blue = static_cast<unsigned char>(182);
    col_b58db6.alpha = static_cast<unsigned char>(255);

    // c5afa0 rgb(197, 175, 160)
    PixelColor col_c5afa0;
    col_c5afa0.red = static_cast<unsigned char>(197);
    col_c5afa0.green = static_cast<unsigned char>(175);
    col_c5afa0.blue = static_cast<unsigned char>(160);
    col_c5afa0.alpha = static_cast<unsigned char>(255);
    // e9bcb7 rgb(233, 188, 183)
    PixelColor col_e9bcb7;
    col_e9bcb7.red = static_cast<unsigned char>(233);
    col_e9bcb7.green = static_cast<unsigned char>(188);
    col_e9bcb7.blue = static_cast<unsigned char>(183);
    col_e9bcb7.alpha = static_cast<unsigned char>(255);
    // ab756b rgb(171, 117, 107)
    PixelColor col_ab756b;
    col_ab756b.red = static_cast<unsigned char>(171);
    col_ab756b.green = static_cast<unsigned char>(117);
    col_ab756b.blue = static_cast<unsigned char>(107);
    col_ab756b.alpha = static_cast<unsigned char>(255);

    // 0081af rgb(0, 129, 175)
    PixelColor col_0081af;
    col_0081af.red = static_cast<unsigned char>(0);
    col_0081af.green = static_cast<unsigned char>(129);
    col_0081af.blue = static_cast<unsigned char>(175);
    col_0081af.alpha = static_cast<unsigned char>(255);
    // 00abe7 rgb(0, 171, 231)
    PixelColor col_00abe7;
    col_00abe7.red = static_cast<unsigned char>(0);
    col_00abe7.green = static_cast<unsigned char>(171);
    col_00abe7.blue = static_cast<unsigned char>(231);
    col_00abe7.alpha = static_cast<unsigned char>(255);
    // 2dc7ff rgb(45, 199, 255)
    PixelColor col_2dc7ff;
    col_2dc7ff.red = static_cast<unsigned char>(45);
    col_2dc7ff.green = static_cast<unsigned char>(199);
    col_2dc7ff.blue = static_cast<unsigned char>(255);
    col_2dc7ff.alpha = static_cast<unsigned char>(255);

    // 06070e rgb(6, 7, 14)
    PixelColor col_06070e;
    col_06070e.red = static_cast<unsigned char>(6);
    col_06070e.green = static_cast<unsigned char>(7);
    col_06070e.blue = static_cast<unsigned char>(14);
    col_06070e.alpha = static_cast<unsigned char>(255);
    // 29524a rgb(41, 82, 74)
    PixelColor col_29524a;
    col_29524a.red = static_cast<unsigned char>(41);
    col_29524a.green = static_cast<unsigned char>(82);
    col_29524a.blue = static_cast<unsigned char>(74);
    col_29524a.alpha = static_cast<unsigned char>(255);
    // 94a187 rgb(148, 161, 135)
    PixelColor col_94a187;
    col_94a187.red = static_cast<unsigned char>(148);
    col_94a187.green = static_cast<unsigned char>(161);
    col_94a187.blue = static_cast<unsigned char>(135);
    col_94a187.alpha = static_cast<unsigned char>(255);
    // 93acb5 rgb(147, 172, 181)
    PixelColor col_93acb5;
    col_93acb5.red = static_cast<unsigned char>(147);
    col_93acb5.green = static_cast<unsigned char>(172);
    col_93acb5.blue = static_cast<unsigned char>(181);
    col_93acb5.alpha = static_cast<unsigned char>(255);
    // 96c5f7 rgb(150, 197, 247)
    PixelColor col_96c5f7;
    col_96c5f7.red = static_cast<unsigned char>(150);
    col_96c5f7.green = static_cast<unsigned char>(197);
    col_96c5f7.blue = static_cast<unsigned char>(247);
    col_96c5f7.alpha = static_cast<unsigned char>(255);
    // a9d3ff rgb(169, 211, 255)
    PixelColor col_a9d3ff;
    col_a9d3ff.red = static_cast<unsigned char>(169);
    col_a9d3ff.green = static_cast<unsigned char>(211);
    col_a9d3ff.blue = static_cast<unsigned char>(255);
    col_a9d3ff.alpha = static_cast<unsigned char>(255);

    // f2f4ff rgb(242, 244, 255)
    PixelColor col_f2f4ff;
    col_f2f4ff.red = static_cast<unsigned char>(242);
    col_f2f4ff.green = static_cast<unsigned char>(244);
    col_f2f4ff.blue = static_cast<unsigned char>(255);
    col_f2f4ff.alpha = static_cast<unsigned char>(255);

    // ead2ac rgb(234, 210, 172)
    PixelColor col_ead2ac;
    col_ead2ac.red = static_cast<unsigned char>(234);
    col_ead2ac.green = static_cast<unsigned char>(210);
    col_ead2ac.blue = static_cast<unsigned char>(172);
    col_ead2ac.alpha = static_cast<unsigned char>(255);
    // eaba6b rgb(234, 186, 107)
    PixelColor col_eaba6b;
    col_eaba6b.red = static_cast<unsigned char>(234);
    col_eaba6b.green = static_cast<unsigned char>(186);
    col_eaba6b.blue = static_cast<unsigned char>(107);
    col_eaba6b.alpha = static_cast<unsigned char>(255);

    // 8ab0ab rgb(138, 176, 171)
    PixelColor col_8ab0ab;
    col_8ab0ab.red = static_cast<unsigned char>(138);
    col_8ab0ab.green = static_cast<unsigned char>(176);
    col_8ab0ab.blue = static_cast<unsigned char>(171);
    col_8ab0ab.alpha = static_cast<unsigned char>(255);
    // 3e505b rgb(62, 80, 91)
    PixelColor col_3e505b;
    col_3e505b.red = static_cast<unsigned char>(62);
    col_3e505b.green = static_cast<unsigned char>(80);
    col_3e505b.blue = static_cast<unsigned char>(91);
    col_3e505b.alpha = static_cast<unsigned char>(255);
    // 26413c rgb(38, 65, 60)
    PixelColor col_26413c;
    col_26413c.red = static_cast<unsigned char>(38);
    col_26413c.green = static_cast<unsigned char>(65);
    col_26413c.blue = static_cast<unsigned char>(60);
    col_26413c.alpha = static_cast<unsigned char>(255);
    // 1a1d1a rgb(26, 29, 26)
    PixelColor col_1a1d1a;
    col_1a1d1a.red = static_cast<unsigned char>(26);
    col_1a1d1a.green = static_cast<unsigned char>(29);
    col_1a1d1a.blue = static_cast<unsigned char>(26);
    col_1a1d1a.alpha = static_cast<unsigned char>(255);
    // 03120e rgb(3, 18, 14)
    PixelColor col_03120e;
    col_03120e.red = static_cast<unsigned char>(3);
    col_03120e.green = static_cast<unsigned char>(18);
    col_03120e.blue = static_cast<unsigned char>(14);
    col_03120e.alpha = static_cast<unsigned char>(255);

    std::vector<PixelColor> rollingInTheDeepColors = {col_e8d6cb, col_d0ada7, col_ad6a6c, col_5d2e46, col_b58db6, col_c5afa0, col_e9bcb7, col_ab756b, col_0081af, col_00abe7, col_2dc7ff, col_06070e, col_29524a, col_94a187, col_93acb5, col_96c5f7, col_a9d3ff, col_f2f4ff, col_ead2ac, col_eaba6b, col_8ab0ab, col_3e505b, col_26413c, col_1a1d1a, col_03120e};

    PixelColor color;
    color.alpha = 255;
    if (count >= limit)
    {
        color.red = color.green = color.blue = 0;
    }
    else
    {
        PixelColor selectedColor = rollingInTheDeepColors[(count + 7 * onsetsPassed) % rollingInTheDeepColors.size()];
        color.red = static_cast<unsigned char>(selectedColor.red);
        color.green = static_cast<unsigned char>(selectedColor.green);
        color.blue = static_cast<unsigned char>(selectedColor.blue);
        color.alpha = static_cast<unsigned char>(selectedColor.alpha);
    }

    return color;
}
