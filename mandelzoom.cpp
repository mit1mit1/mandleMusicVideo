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
#include <cmath>
#include <vector>
#include "lodepng.h"

struct PixelColor
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
};


class VideoFrame
{
private:
    int width;
    int height;
    std::vector<unsigned char> buffer;

public:
    VideoFrame(int _width, int _height)
        : width(_width)
        , height(_height)
        , buffer(4 * _width * _height, 255)
        {}

    void SetPixel(int x, int y, PixelColor color)
    {
        int index = 4 * (y*width + x);
        buffer[index]   = color.red;
        buffer[index+1] = color.green;
        buffer[index+2] = color.blue;
        buffer[index+3] = color.alpha;
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
static int GenerateZoomFrames(const char *outdir, int numframes, double xcenter, double ycenter, double zoom, int framespersecond);
static double GetTimestampSeconds(int framenumber, int framespersecond);
static int Mandelbrot(double cr, double ci, int limit);
static PixelColor Palette(int count, int limit, double timestamp);

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
        if (argc > 6) {
            framespersecond = atoi(argv[6]);
        }
        if (zoom < 1.0)
        {
            fprintf(stderr, "ERROR: zoom factor must be 1.0 or greater.\n");
            return 1;
        }
        return GenerateZoomFrames(outdir, numframes, xcenter, ycenter, zoom, framespersecond);
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
        "\n"
    );

    return 1;
}

static double GetTimestampSeconds(int framenumber, int framespersecond)
{
    return framenumber / framespersecond;
}

static int GenerateZoomFrames(const char *outdir, int numframes, double xcenter, double ycenter, double zoom, int framespersecond = 30)
{
    try
    {
        // Create a video frame buffer with 720p resolution (1280x720).
        const int width  = 1280;
        const int height =  720;
        VideoFrame frame(width, height);

        const int limit = 16000;
        double multiplier = pow(zoom, 1.0 / (numframes - 1.0));
        double denom = 1.0;

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
            double ci_top = ycenter + ver_span/2.0;
            double ci_delta = ver_span / (height - 1.0);
            double cr_left = xcenter - hor_span/2.0;
            double cr_delta = hor_span / (width - 1.0);

            for (int x=0; x < width; ++x)
            {
                double cr = cr_left + x*cr_delta;
                for (int y=0; y < height; ++y)
                {
                    double ci = ci_top - y*ci_delta;
                    int count = Mandelbrot(cr, ci, limit);
                    PixelColor color = Palette(count, limit, timestamp);
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
        double tzi = 2.0*zr*zi + ci;
        zr = zr2 - zi2 + cr;
        zi = tzi;
        zr2 = zr*zr;
        zi2 = zi*zi;
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


static PixelColor Palette(int count, int limit, double timestamp)
{
    PixelColor color;
    double onsetTimestamps [] = {0.017415, 0.117415, 0.911723, 0.950748, 1.109297, 1.189048, 1.223379, 1.336349, 1.376100, 1.799252, 1.948866, 2.019637, 2.364036, 2.532698, 2.567415, 2.903311, 3.142222, 3.732857, 4.092721, 4.321020, 4.602676, 4.884762, 4.911270, 5.166848, 5.192290, 5.495601, 5.760317, 5.789932, 6.065850, 6.088322, 6.368639, 6.546825, 6.672290, 6.922494, 6.959546, 6.980385, 7.079955, 7.240227, 7.476463, 7.524649, 7.813878, 7.834717, 8.085714, 8.104785, 8.416417, 8.673424, 8.694036, 8.763333, 8.811429, 8.971950, 9.218617, 9.249093, 9.522222, 9.687029, 9.734376, 9.768617, 9.807937, 9.831247, 10.113605, 10.395011};
    int onsetsPassed = 1;
    for (double onsetTimestamp : onsetTimestamps)
    {
        if (timestamp > onsetTimestamp) {
            onsetsPassed++;
        }
    }
    if (count >= limit)
    {
        color.red = color.green = color.blue = 0;
    }
    else
    {
        double x = static_cast<double>(count) / (limit - 1.0);
        color.red   = static_cast<unsigned char>(255.0 * ZigZag(0.5 + 7.0*x + 0.13*onsetsPassed));
        color.green = static_cast<unsigned char>(255.0 * ZigZag(0.2 + 9.0*x + 0.15*onsetsPassed));
        color.blue  = static_cast<unsigned char>(255.0 * ZigZag(0.7 + 11.0*x + 0.19*onsetsPassed));
    }
    color.alpha = 255;

    return color;
}

