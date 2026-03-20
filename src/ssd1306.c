#include "driver/i2c.h"

#include "ssd1306.h"
#include "constants.h"

static uint8_t frame_buffer[SSD1306_WIDTH * SSD1306_PAGES];

static inline void ssd1306_write_data(const uint8_t *data, size_t data_length)
{
    //create i2c command handle
    i2c_cmd_handle_t i2c_command_handle = i2c_cmd_link_create();

    //pull data line from high to low (clock stays high)
    i2c_master_start(i2c_command_handle);

    //shift 7 bit i2c address to right and set lsb of byte as zero for i2c write
    i2c_master_write_byte(i2c_command_handle, (SSD1306_ADDRESS << 1) | I2C_MASTER_WRITE, I2C_ACK_ENABLE);

    //send 0x00 or 0x40 to tell display if it is a command or data write, then send the data/command itself
    i2c_master_write_byte(i2c_command_handle, SSD1306_DATA, I2C_ACK_ENABLE);
    i2c_master_write(i2c_command_handle, data, data_length, I2C_ACK_ENABLE);

    //pull data line from low to high (clock stays high)
    i2c_master_stop(i2c_command_handle);

    //actually send command to bus
    i2c_master_cmd_begin(I2C_PORT, i2c_command_handle, pdMS_TO_TICKS(I2C_TIMEOUT_MS));

    //delete the i2c command handle
    i2c_cmd_link_delete(i2c_command_handle);
}

static inline void ssd1306_write_command(const uint8_t command)
{
    //create i2c command handle
    i2c_cmd_handle_t i2c_command_handle = i2c_cmd_link_create();

    //pull data line from high to low (clock stays high)
    i2c_master_start(i2c_command_handle);

    //shift 7 bit i2c address to right and set lsb of byte as zero for i2c write
    i2c_master_write_byte(i2c_command_handle, (SSD1306_ADDRESS << 1) | I2C_MASTER_WRITE, I2C_ACK_ENABLE);

    //send 0x00 or 0x40 to tell display if it is a command or data write, then send the data/command itself
    i2c_master_write_byte(i2c_command_handle, SSD1306_CMD, I2C_ACK_ENABLE);
    i2c_master_write_byte(i2c_command_handle, command, I2C_ACK_ENABLE);

    //pull data line from low to high (clock stays high)
    i2c_master_stop(i2c_command_handle);

    //actually send command to bus
    i2c_master_cmd_begin(I2C_PORT, i2c_command_handle, pdMS_TO_TICKS(I2C_TIMEOUT_MS));

    //delete the i2c command handle
    i2c_cmd_link_delete(i2c_command_handle);
}

void ssd1306_init(ssd1306_t* display, uint8_t i2c_address, uint32_t i2c_clock, uint8_t pin_sda, uint8_t pin_scl, uint16_t width, uint16_t height)
{
    display->address     = i2c_address;
    display->clock_speed = i2c_clock;

    display->pin_sda = pin_sda;
    display->pin_scl = pin_scl;
    display->width   = width;
    display->height  = height;

    i2c_config_t i2c_config = {
        .mode             = I2C_MODE_MASTER,
        .sda_io_num       = display->pin_sda,
        .scl_io_num       = display->pin_scl,
        .sda_pullup_en    = GPIO_PULLUP_ENABLE,
        .scl_pullup_en    = GPIO_PULLUP_ENABLE,
        .master.clk_speed = display->clock_speed,
    };

    i2c_param_config  (I2C_PORT, &i2c_config);
    i2c_driver_install(I2C_PORT, i2c_config.mode, 0, 0, 0);

    ssd1306_write_command(0xAE); // display off
    ssd1306_write_command(0x20); ssd1306_write_command(0x00); // horizontal addressing
    ssd1306_write_command(0xB0); // page 0
    ssd1306_write_command(0xC8); // COM scan direction
    ssd1306_write_command(0x00); // low col
    ssd1306_write_command(0x10); // high col
    ssd1306_write_command(0x40); // start line
    ssd1306_write_command(0x81); ssd1306_write_command(0x7F); // contrast
    ssd1306_write_command(0xA1); // segment remap
    ssd1306_write_command(0xA6); // normal display
    ssd1306_write_command(0xA8); ssd1306_write_command(0x1F); // multiplex = 32 rows
    ssd1306_write_command(0xA4); // display RAM
    ssd1306_write_command(0xD3); ssd1306_write_command(0x00); // offset
    ssd1306_write_command(0xD5); ssd1306_write_command(0x80); // clock
    ssd1306_write_command(0xD9); ssd1306_write_command(0xF1); // precharge
    ssd1306_write_command(0xDA); ssd1306_write_command(0x02); // COM pins hardware config for 32-row display
    ssd1306_write_command(0xDB); ssd1306_write_command(0x40); // vcom detect
    ssd1306_write_command(0x8D); ssd1306_write_command(0x14); // charge pump
    ssd1306_write_command(0xAF); // display ON

    ssd1306_clear();
    ssd1306_update(display);
}

void ssd1306_clear(void)
{
    for(int i = 0; i < sizeof(frame_buffer); i++)
    {
        frame_buffer[i] = 0x00;
    }
}

void ssd1306_update(ssd1306_t* display)
{
    for(uint8_t page = 0; page < SSD1306_PAGES; page++)
    {
        ssd1306_write_command(SSD1306_PAGE_START_ADDRESS + page);

        ssd1306_write_data(&frame_buffer[display->width * page], display->width);
    }
}

void ssd1306_draw_pixel(ssd1306_t* display, uint8_t x, uint8_t y, bool color)
{
    if (x >= display->width || y >= display->height) return;

    uint16_t index = x + (y / 8) * display->width;

    uint8_t bit    = 1 << (y & 0b0111);

    if(color)
        frame_buffer[index] |=  bit;
    else
        frame_buffer[index] &= ~bit;
}

void ssd1306_draw_hline(ssd1306_t* display, uint8_t x, uint8_t y, uint16_t width, bool color) 
{
    if (x >= display->width || y >= display->height) return;

    uint16_t end = x + width;

    if(end > display->width) end = display->width;

    for(uint16_t i = x; i < end; i++) 
    {
        ssd1306_draw_pixel(display, i, y, color);
    }
}

void ssd1306_draw_vline(ssd1306_t* display, uint8_t x, uint8_t y, uint16_t height, bool color) 
{
    if (x >= display->width || y >= display->height) return;

    uint16_t end = y + height;

    if(end > display->height) end = display->height;

    for(uint16_t i = y; i < end; i++) 
    {
        ssd1306_draw_pixel(display, x, i, color);
    }
}

void ssd1306_draw_fill(ssd1306_t* display, bool color){
    if(color)
    {
        for(int i = 0; i < sizeof(frame_buffer); i++) frame_buffer[i] = 0xff;
    }
    else ssd1306_clear();
}

void ssd1306_draw_char(ssd1306_t* display, char character, uint8_t x, uint8_t y_page)
{
    if(character < 0 || character > 127) return;
    
    if(x >= display->width || y_page >= SSD1306_PAGES) return;

    for(uint8_t col = 0; col < 8; col++){

        uint16_t index = x + y_page * display->width;

        if(index < display->width * SSD1306_PAGES)
        {
            frame_buffer[index + col] = font8x8[(uint8_t)character][col];
        }
    }
}

void ssd1306_draw_string(ssd1306_t* display, char* string, uint8_t x, uint8_t y_page)
{
    uint8_t cursor_x = x;

    for(size_t i = 0; string[i] != '\0'; i++)
    {
        if(cursor_x + 8 > display->width) break; 

        ssd1306_draw_char(display, string[i], cursor_x, y_page);
        cursor_x += 8; 
    }
}

void ssd1306_draw_circle(ssd1306_t* display, int16_t x0, int16_t y0, int16_t radius, bool color)
{
    int16_t x = radius;
    int16_t y = 0;
    int16_t err = 1 - x;

    while (x >= y)
    {
        ssd1306_draw_pixel(display, x0 + x, y0 + y, color);
        ssd1306_draw_pixel(display, x0 + y, y0 + x, color);
        ssd1306_draw_pixel(display, x0 - y, y0 + x, color);
        ssd1306_draw_pixel(display, x0 - x, y0 + y, color);
        ssd1306_draw_pixel(display, x0 - x, y0 - y, color);
        ssd1306_draw_pixel(display, x0 - y, y0 - x, color);
        ssd1306_draw_pixel(display, x0 + y, y0 - x, color);
        ssd1306_draw_pixel(display, x0 + x, y0 - y, color);

        y++;

        if (err < 0)
        {
            err += 2 * y + 1;
        }
        else
        {
            x--;
            err += 2 * (y - x + 1);
        }
    }
}