#include "ssd1306.h"

// --- Private Module Variables ---

// Screen buffer in RAM. Each byte represents a vertical column of 8 pixels.
static uint8_t g_ssd1306_buffer[SSD1306_BUFFER_SIZE];

// I2C port used for communication
static i2c_t* i2c_port = NULL;

// --- Private Helper Functions ---

/**
 * @brief Sends a single command byte to the SSD1306.
 * @param[in] cmd The command byte to send.
 */
static void ssd1306_write_command(uint8_t cmd) {
    if (i2c_port == NULL) return;
    // The command sequence is [Control Byte (0x00), Command Byte]
    uint8_t buffer[2] = {0x00, cmd};
    i2c_master_write(i2c_port, SSD1306_I2C_ADDR, buffer, 2);
}

// --- Public API Implementation ---

/**
 * @brief Initializes the SSD1306 display.
 */
bool ssd1306_init(i2c_t *I2Cx) {
    if (I2Cx == NULL) return false;
    i2c_port = I2Cx;

    // A short delay is good practice before starting initialization
    systick_delay_ms(100);

    // Standard initialization sequence for a 128x64 SSD1306
    ssd1306_write_command(0xAE); // Display OFF
    ssd1306_write_command(0x20); // Set Memory Addressing Mode
    ssd1306_write_command(0x10); // 00,Horizontal Addressing Mode; 01,Vertical Addressing Mode; 10,Page Addressing Mode (RESET); 11,Invalid
    ssd1306_write_command(0xB0); // Set Page Start Address for Page Addressing Mode, 0-7
    ssd1306_write_command(0xC8); // Set COM Output Scan Direction
    ssd1306_write_command(0x00); // ---set low column address
    ssd1306_write_command(0x10); // ---set high column address
    ssd1306_write_command(0x40); // --set start line address
    ssd1306_write_command(0x81); // --set contrast control register
    ssd1306_write_command(0xFF);
    ssd1306_write_command(0xA1); // --set segment re-map 0 to 127
    ssd1306_write_command(0xA6); // --set normal display
    ssd1306_write_command(0xA8); // --set multiplex ratio(1 to 64)
    ssd1306_write_command(0x3F); //
    ssd1306_write_command(0xA4); // 0xa4,Output follows RAM content; 0xa5,Output ignores RAM content
    ssd1306_write_command(0xD3); // -set display offset
    ssd1306_write_command(0x00); // -not offset
    ssd1306_write_command(0xD5); // --set display clock divide ratio/oscillator frequency
    ssd1306_write_command(0xF0); // --set divide ratio
    ssd1306_write_command(0xD9); // --set pre-charge period
    ssd1306_write_command(0x22); //
    ssd1306_write_command(0xDA); // --set com pins hardware configuration
    ssd1306_write_command(0x12);
    ssd1306_write_command(0xDB); // --set vcomh
    ssd1306_write_command(0x20); // 0x20,0.77xVcc
    ssd1306_write_command(0x8D); // --set DC-DC enable
    ssd1306_write_command(0x14); //
    ssd1306_write_command(0xAF); // --turn on SSD1306 panel

    // Clear the screen buffer and update the display
    ssd1306_fill(SSD1306_COLOR_BLACK);
    ssd1306_update_screen();

    return true;
}

/**
 * @brief Fills the entire screen buffer with a specified color.
 */
void ssd1306_fill(ssd1306_color_t color) {
    uint8_t fill_val = (color == SSD1306_COLOR_BLACK) ? 0x00 : 0xFF;
    for (uint16_t i = 0; i < SSD1306_BUFFER_SIZE; i++) {
        g_ssd1306_buffer[i] = fill_val;
    }
}

/**
 * @brief Updates the physical screen with the contents of the screen buffer.
 */
void ssd1306_update_screen(void) {
    if (i2c_port == NULL) return;

    // Set the display cursor to the top-left corner
    ssd1306_write_command(0x21); // Set Column Address
    ssd1306_write_command(0);    // Column start
    ssd1306_write_command(127);  // Column end
    ssd1306_write_command(0x22); // Set Page Address
    ssd1306_write_command(0);    // Page start
    ssd1306_write_command(3);    // Page end (8 pages for 64 rows)

    // The data transfer needs a control byte (0x40) followed by the data.
    // We create a temporary buffer to hold the control byte and a chunk of data.
    uint8_t data_chunk[17]; // 1 control byte + 16 data bytes
    data_chunk[0] = 0x40;   // Data control byte

    for (uint16_t i = 0; i < SSD1306_BUFFER_SIZE; i += 16) {
        for (uint8_t j = 0; j < 16; j++) {
            data_chunk[j + 1] = g_ssd1306_buffer[i + j];
        }
        i2c_master_write(i2c_port, SSD1306_I2C_ADDR, data_chunk, sizeof(data_chunk));
    }
}

/**
 * @brief Draws a single pixel in the screen buffer.
 */
void ssd1306_draw_pixel(uint8_t x, uint8_t y, ssd1306_color_t color) {
    // Check for out-of-bounds coordinates
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        return;
    }

    // Calculate the position in the buffer
    uint16_t buffer_index = x + (y / 8) * SSD1306_WIDTH;
    uint8_t bit_pos = y % 8;

    if (color == SSD1306_COLOR_WHITE) {
        g_ssd1306_buffer[buffer_index] |= (1 << bit_pos);
    } else {
        g_ssd1306_buffer[buffer_index] &= ~(1 << bit_pos);
    }
}

/**
 * @brief Draws a single character at the specified position.
 */
uint8_t ssd1306_draw_char(uint8_t x, uint8_t y, char c, ssd1306_color_t color) {
    // Check for printable ASCII range
    if (c < ' ' || c > '~') {
        return x;
    }

    // Subtract space character to get index in font array
    const uint8_t* font_char = g_font_5x7[c - ' '];

    // Draw the 5x7 character
    for (uint8_t col = 0; col < 5; col++) {
        uint8_t line = font_char[col];
        for (uint8_t row = 0; row < 7; row++) {
            if ((line >> row) & 1) {
                ssd1306_draw_pixel(x + col, y + row, color);
            }
        }
    }
    // Return the new x-position for the next character (5px width + 1px space)
    return x + 6;
}

/**
 * @brief Draws a string of characters at the specified position.
 */
void ssd1306_draw_string(uint8_t x, uint8_t y, const char* str, ssd1306_color_t color) {
    uint8_t current_x = x;
    while (*str) {
        current_x = ssd1306_draw_char(current_x, y, *str++, color);
        // Basic word wrapping
        if (current_x + 5 > SSD1306_WIDTH) {
            current_x = x;
            y += 8; // Move to next line
        }
    }
}
