#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdint.h>
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"

typedef struct{
    adc_channel_t adc_channel_pin_x;
    adc_channel_t adc_channel_pin_y;

    adc_oneshot_unit_handle_t adc_handle_pin_x;
    adc_oneshot_unit_handle_t adc_handle_pin_y;

    uint16_t x;
    uint16_t y;
    uint8_t  state;

    uint8_t pin_x;
    uint8_t pin_y;
    uint8_t pin_sw;
} joystick_t;

void joystick_init(joystick_t* joystick, gpio_num_t pin_x, gpio_num_t pin_y, gpio_num_t pin_sw);
void joystick_read(joystick_t* joystick);

int16_t joystick_normalize(int16_t raw, int16_t center, int16_t range_negative, int16_t range_positive, int16_t deadzone);
float   joystick_map(float x, float in_min, float in_max, float out_min, float out_max);
int16_t joystick_constrain(int16_t in, int16_t out_min, int16_t out_max);

#endif