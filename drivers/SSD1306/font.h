#ifndef FONT_H
#define FONT_H

#include <stdint.h>

// This array holds the bitmap data for a 5x7 pixel font.
// Each character is represented by 5 bytes (5 columns, 7 rows).
extern const uint8_t g_font_5x7[95][5];

#endif // FONT_H
