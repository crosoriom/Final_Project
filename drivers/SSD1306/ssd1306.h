#ifndef SSD1306_H
#define SSD1306_H

#include <stdbool.h>
#include "systick.h"
#include "font.h"
#include "i2c.h" // For i2c_port_t

// --- Configuration and Constants ---
#define SSD1306_I2C_ADDR   (0x3C) // Default I2C address for many 128x64 displays
#define SSD1306_WIDTH      (128)
#define SSD1306_HEIGHT     (32)
#define SSD1306_BUFFER_SIZE (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

// --- Color Enum ---
// Monochrome display: only two "colors"
typedef enum {
    SSD1306_COLOR_BLACK = 0, // Pixel is off
    SSD1306_COLOR_WHITE = 1  // Pixel is on
} ssd1306_color_t;


// --- Public API Functions ---

/**
 * @brief Initializes the SSD1306 display.
 * @param[in] i2c_port Pointer to the initialized I2C peripheral to use.
 * @return true on success, false on failure.
 */
bool ssd1306_init(i2c_t *i2c_port);

/**
 * @brief Fills the entire screen buffer with a specified color.
 * @param[in] color The color to fill the screen with (BLACK or WHITE).
 */
void ssd1306_fill(ssd1306_color_t color);

/**
 * @brief Updates the physical screen with the contents of the screen buffer.
 */
void ssd1306_update_screen(void);

/**
 * @brief Draws a single pixel in the screen buffer.
 * @param[in] x The x-coordinate (0-127).
 * @param[in] y The y-coordinate (0-63).
 * @param[in] color The color of the pixel (BLACK or WHITE).
 */
void ssd1306_draw_pixel(uint8_t x, uint8_t y, ssd1306_color_t color);

/**
 * @brief Draws a single character at the specified position.
 * @param[in] x The starting x-coordinate of the character.
 * @param[in] y The starting y-coordinate of the character.
 * @param[in] c The character to draw.
 * @param[in] color The color of the character.
 * @return The x-coordinate for the next character.
 */
uint8_t ssd1306_draw_char(uint8_t x, uint8_t y, char c, ssd1306_color_t color);

/**
 * @brief Draws a string of characters at the specified position.
 * @param[in] x The starting x-coordinate of the string.
 * @param[in] y The starting y-coordinate of the string.
 * @param[in] str The null-terminated string to draw.
 * @param[in] color The color of the string.
 */
void ssd1306_draw_string(uint8_t x, uint8_t y, const char* str, ssd1306_color_t color);

#endif // SSD1306_H
