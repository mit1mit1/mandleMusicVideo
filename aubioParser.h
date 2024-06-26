#ifndef MANDLEAUBIO_H // To make sure you don't declare the function more than once by including the header multiple times.
#define MANDLEAUBIO_H

#include "structs.h"
#include <vector>

std::vector<AubioNote> ParseAubioNoteFile(const char *filename, float extraSeconds);
std::vector<float> ParseOnsetSecondsFile(const char *filename);

#endif
