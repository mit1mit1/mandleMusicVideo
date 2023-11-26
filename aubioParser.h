#ifndef MANDLEAUBIO_H // To make sure you don't declare the function more than once by including the header multiple times.
#define MANDLEAUBIO_H

#include "structs.h"

std::vector<AubioNote> ParseAubioNoteFile(const char *filename);
std::vector<float> ParseOnsetSecondsFile(const char *filename);

#endif
