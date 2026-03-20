#include <math.h>

#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"

#include "constants.h"
#include "adc.h"
#include "joystick.h"

static adc_unit_t    gpio_to_adc_unit   (gpio_num_t gpio);
static adc_channel_t gpio_to_adc_channel(gpio_num_t gpio);

void  joystick_init(joystick_t* joystick, gpio_num_t pin_x, gpio_num_t pin_y, gpio_num_t pin_sw);
void  joystick_read(joystick_t* joystick);
float joystick_map(float x, float in_min, float in_max, float out_min, float out_max);

void joystick_init(joystick_t* joystick, gpio_num_t pin_x, gpio_num_t pin_y, gpio_num_t pin_sw){
    joystick->pin_x  = pin_x;
    joystick->pin_y  = pin_y;
    joystick->pin_sw = pin_sw;

    joystick->adc_handle_pin_x = adc_unit_to_handle(gpio_to_adc_unit(pin_x));
    joystick->adc_handle_pin_y = adc_unit_to_handle(gpio_to_adc_unit(pin_y));

    adc_oneshot_chan_cfg_t channel_config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten    = ADC_ATTEN_DB_12  
    };

    joystick->adc_channel_pin_x = gpio_to_adc_channel(pin_x);
    adc_oneshot_config_channel(joystick->adc_handle_pin_x, joystick->adc_channel_pin_x, &channel_config);

    joystick->adc_channel_pin_y = gpio_to_adc_channel(pin_y);
    adc_oneshot_config_channel(joystick->adc_handle_pin_y, joystick->adc_channel_pin_y, &channel_config);

    gpio_config_t config_pin_sw = {
        .pin_bit_mask = 1ULL << pin_sw,
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE
    };

    gpio_config(&config_pin_sw);

    joystick->x     = 0;
    joystick->y     = 0;
    joystick->state = 0;
}

void joystick_read(joystick_t* joystick){
    int raw_x, raw_y;

    adc_oneshot_read(joystick->adc_handle_pin_x, joystick->adc_channel_pin_x, &raw_x);
    adc_oneshot_read(joystick->adc_handle_pin_y, joystick->adc_channel_pin_y, &raw_y);

    joystick->x = (uint16_t)raw_x;
    joystick->y = (uint16_t)raw_y;
    
    joystick->state = !gpio_get_level(joystick->pin_sw);
}

int16_t joystick_normalize(int16_t raw, int16_t center, int16_t range_negative, int16_t range_positive, int16_t deadzone){
    int value = raw - center;

    if(abs(value) < deadzone) return 0;

    if(value < 0)
        value = (int16_t)joystick_map(value, -range_negative, 0, MIN_TRANSMITTER, MID_TRANSMITTER);
    else
        value = (int16_t)joystick_map(value, 0, range_positive, MID_TRANSMITTER, MAX_TRANSMITTER);

    return value = joystick_constrain(value, MIN_TRANSMITTER, MAX_TRANSMITTER);
}

float joystick_map(float x, float in_min, float in_max, float out_min, float out_max){
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int16_t joystick_constrain(int16_t in, int16_t out_min, int16_t out_max){
    if(in < out_min){
        return out_min;
    }
    else if(in > out_max){
        return out_max;
    }
    else return in;
}

static adc_unit_t gpio_to_adc_unit(gpio_num_t gpio){
    switch (gpio){
        case GPIO_NUM_36: 
        case GPIO_NUM_37: 
        case GPIO_NUM_38: 
        case GPIO_NUM_39:
        case GPIO_NUM_32: 
        case GPIO_NUM_33: 
        case GPIO_NUM_34: 
        case GPIO_NUM_35: return ADC_UNIT_1;

        case GPIO_NUM_0: 
        case GPIO_NUM_2: 
        case GPIO_NUM_4: 
        case GPIO_NUM_12:
        case GPIO_NUM_13:
        case GPIO_NUM_14: 
        case GPIO_NUM_15:
        case GPIO_NUM_25: 
        case GPIO_NUM_26: 
        case GPIO_NUM_27: return ADC_UNIT_2;

        default: return -1; 
    }
}

static adc_channel_t gpio_to_adc_channel(gpio_num_t gpio){
    switch(gpio){
        case GPIO_NUM_36: return ADC_CHANNEL_0;
        case GPIO_NUM_37: return ADC_CHANNEL_1;
        case GPIO_NUM_38: return ADC_CHANNEL_2;
        case GPIO_NUM_39: return ADC_CHANNEL_3;
        case GPIO_NUM_32: return ADC_CHANNEL_4;
        case GPIO_NUM_33: return ADC_CHANNEL_5;
        case GPIO_NUM_34: return ADC_CHANNEL_6;
        case GPIO_NUM_35: return ADC_CHANNEL_7;
        case GPIO_NUM_0 : return ADC_CHANNEL_1;
        case GPIO_NUM_2 : return ADC_CHANNEL_2;
        case GPIO_NUM_4 : return ADC_CHANNEL_0;
        case GPIO_NUM_12: return ADC_CHANNEL_5;
        case GPIO_NUM_13: return ADC_CHANNEL_4;
        case GPIO_NUM_14: return ADC_CHANNEL_6;
        case GPIO_NUM_15: return ADC_CHANNEL_3;
        case GPIO_NUM_25: return ADC_CHANNEL_8;
        case GPIO_NUM_26: return ADC_CHANNEL_9;
        case GPIO_NUM_27: return ADC_CHANNEL_7;
        default: return -1; 
    }
}
