#ifndef SSD1306_H
#define SSD1306_H

#include "ssd1306_font.h"
#include <stdint.h>

#define SSD1306_ADDRESS   0x3c
#define SSD1306_CMD       0x00
#define SSD1306_DATA      0x40
#define SSD1306_BLACK     0
#define SSD1306_WHITE     1
#define SSD1306_WIDTH     128
#define SSD1306_HEIGHT    32
#define SSD1306_PAGE_SIZE 8
#define SSD1306_PAGES     4

#define SSD1306_PAGE_START_ADDRESS 0xB0

#define I2C_PORT        I2C_NUM_0
#define I2C_FREQUENCY   400000
#define I2C_TIMEOUT_MS  150
#define I2C_ACK_ENABLE  1

typedef struct{
    uint32_t clock_speed;
    uint16_t width;
    uint16_t height;
    uint8_t  pin_sda;
    uint8_t  pin_scl;
    uint8_t  address;
} ssd1306_t;

void ssd1306_init(ssd1306_t* display, uint8_t i2c_address, uint32_t i2c_clock, uint8_t pin_sda, uint8_t pin_scl, uint16_t width, uint16_t height);
void ssd1306_clear(void);
void ssd1306_update(ssd1306_t* display);
void ssd1306_draw_pixel(ssd1306_t* display, uint8_t x, uint8_t y, bool color);
void ssd1306_draw_circle(ssd1306_t* display, int16_t x0, int16_t y0, int16_t radius, bool color);
void ssd1306_draw_hline(ssd1306_t* display, uint8_t x, uint8_t y, uint16_t width, bool color);
void ssd1306_draw_vline(ssd1306_t* display, uint8_t x, uint8_t y, uint16_t height, bool color);
void ssd1306_draw_fill(ssd1306_t* display, bool color);
void ssd1306_draw_char(ssd1306_t* display, char character, uint8_t x, uint8_t y_page);
void ssd1306_draw_string(ssd1306_t* display, char* string, uint8_t x, uint8_t y_page);

#endif