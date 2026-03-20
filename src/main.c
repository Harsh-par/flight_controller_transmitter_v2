#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/ledc.h"

#include "esp_task_wdt.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "constants.h"
#include "globals.h"
#include "ssd1306.h"
#include "joystick.h"
#include "adc.h"
#include "transmitter.h"

void task_core0();
void task_core1();

joystick_t joystick_l;
joystick_t joystick_m;
joystick_t joystick_r;

controller_data_t transmitter;

ssd1306_t ssd1306;

int64_t time_us_current;
int64_t time_us_previous;
float   time_us_delta; 

bool tx_connected_to_rx = false;

void app_main(void){

    adc_handle_init();

    joystick_init(&joystick_l, PIN_JS_LX, PIN_JS_LY, PIN_JS_LSW);
    joystick_init(&joystick_m, PIN_JS_MX, PIN_JS_MY, PIN_JS_MSW);
    joystick_init(&joystick_r, PIN_JS_RX, PIN_JS_RY, PIN_JS_RSW);

    transmitter_init(&transmitter);

    xTaskCreatePinnedToCore(task_core0, "core0", TASK_STACK_SIZE, NULL, 2, NULL, ESP_CORE_0);
    xTaskCreatePinnedToCore(task_core1, "core1", TASK_STACK_SIZE, NULL, 2, NULL, ESP_CORE_1);
}

void task_core0(){

    time_us_previous = esp_timer_get_time();

    while(true)
    {
        time_us_current = esp_timer_get_time();
        time_us_delta   = (time_us_current - time_us_previous) * MICROSECOND_TO_SECOND;
        
        joystick_read(&joystick_l);
        joystick_read(&joystick_m);
        joystick_read(&joystick_r);
        
        transmitter_set(&transmitter, &joystick_l, &joystick_m, &joystick_r);
        transmitter_send(&transmitter);

        time_us_previous = time_us_current;

        vTaskDelay(TASK_DELAY_MS / portTICK_PERIOD_MS);
    }
}

void task_core1(){

    ssd1306_init(&ssd1306, SSD1306_ADDRESS, I2C_FREQUENCY, PIN_I2C_SDA, PIN_I2C_SCL, SSD1306_WIDTH, SSD1306_HEIGHT);

    #define POSITION_X_JS_L 21+22
    #define POSITION_X_JS_M 64+11
    #define POSITION_X_JS_R 106
    #define POSITION_Y_JS   11
    #define RADIUS_JS       11
    #define RADIUS_JS_INNER 2
    
    while(true)
    {
        ssd1306_clear();
        
        ssd1306_draw_circle(&ssd1306, POSITION_X_JS_L, POSITION_Y_JS, RADIUS_JS, SSD1306_WHITE);
        ssd1306_draw_circle(&ssd1306, POSITION_X_JS_M, POSITION_Y_JS, RADIUS_JS, SSD1306_WHITE);
        ssd1306_draw_circle(&ssd1306, POSITION_X_JS_R, POSITION_Y_JS, RADIUS_JS, SSD1306_WHITE);

        ssd1306_draw_circle(&ssd1306, POSITION_X_JS_L, POSITION_Y_JS, 1, transmitter.button_l);
        ssd1306_draw_pixel(&ssd1306,  POSITION_X_JS_L, POSITION_Y_JS, transmitter.button_l);

        ssd1306_draw_circle(&ssd1306, POSITION_X_JS_M, POSITION_Y_JS, 1, transmitter.button_m);
        ssd1306_draw_pixel(&ssd1306,  POSITION_X_JS_M, POSITION_Y_JS, transmitter.button_m);

        ssd1306_draw_circle(&ssd1306, POSITION_X_JS_R, POSITION_Y_JS, 1, transmitter.button_r);
        ssd1306_draw_pixel(&ssd1306,  POSITION_X_JS_R, POSITION_Y_JS, transmitter.button_r);

        uint8_t ly = (uint8_t)joystick_map(transmitter.joystick_ly, MIN_TRANSMITTER, MAX_TRANSMITTER, POSITION_Y_JS + RADIUS_JS - 2, POSITION_Y_JS - RADIUS_JS + 2);
        uint8_t my = (uint8_t)joystick_map(transmitter.joystick_my, MIN_TRANSMITTER, MAX_TRANSMITTER, POSITION_Y_JS + RADIUS_JS - 2, POSITION_Y_JS - RADIUS_JS + 2);
        uint8_t ry = (uint8_t)joystick_map(transmitter.joystick_ry, MIN_TRANSMITTER, MAX_TRANSMITTER, POSITION_Y_JS + RADIUS_JS - 2, POSITION_Y_JS - RADIUS_JS + 2);

        uint8_t lx = (uint8_t)joystick_map(transmitter.joystick_lx, MIN_TRANSMITTER, MAX_TRANSMITTER, POSITION_X_JS_L - RADIUS_JS + 2, POSITION_X_JS_L + RADIUS_JS - 2);
        uint8_t mx = (uint8_t)joystick_map(transmitter.joystick_mx, MIN_TRANSMITTER, MAX_TRANSMITTER, POSITION_X_JS_M - RADIUS_JS + 2, POSITION_X_JS_M + RADIUS_JS - 2);
        uint8_t rx = (uint8_t)joystick_map(transmitter.joystick_rx, MIN_TRANSMITTER, MAX_TRANSMITTER, POSITION_X_JS_R - RADIUS_JS + 2, POSITION_X_JS_R + RADIUS_JS - 2);

        ssd1306_draw_circle(&ssd1306, lx, ly, RADIUS_JS_INNER, SSD1306_WHITE);
        ssd1306_draw_circle(&ssd1306, mx, my, RADIUS_JS_INNER, SSD1306_WHITE);
        ssd1306_draw_circle(&ssd1306, rx, ry, RADIUS_JS_INNER, SSD1306_WHITE);

        ssd1306_draw_string(&ssd1306, "RX", 0, 0);
        if(tx_connected_to_rx){
            ssd1306_draw_string(&ssd1306, "[o]", 0, 1);
        } 
        else ssd1306_draw_string(&ssd1306, "[ ]", 0, 1);
        ssd1306_draw_string(&ssd1306, "transmitter", 0, 3);
        ssd1306_update(&ssd1306);

        vTaskDelay(TASK_DELAY_MS / portTICK_PERIOD_MS);
    }

}
