#include <vector>
#include <cmath>
#include "aubioParser.h"
#include <iostream>
#include <fstream>

std::vector<AubioNote> ParseAubioNoteFile(const char *filename, float extraSeconds)
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
        lineNote.endSeconds = x3 + extraSeconds;
        notes.push_back(lineNote);
    }

    std::cout << "First Note: ";
    std::cout << notes.front().startSeconds << " start sec  " << notes.front().endSeconds << " end sec  " << notes.front().pitch << " pitch ";

    std::cout << "Last Note: ";
    std::cout << notes.back().startSeconds << " start sec  " << notes.back().endSeconds << " end sec  " << notes.back().pitch << " pitch ";

    return notes;
}

// TODO for both onsets and notes keep track of total number of notes and get the average length between notes
std::vector<float> ParseOnsetSecondsFile(const char *filename)
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