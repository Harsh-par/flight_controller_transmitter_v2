#ifndef CONSTANTS_H
#define CONSTANTS_H

#define RADIAN_TO_DEGREE      57.2957795
#define MICROSECOND_TO_SECOND 1e-6f

#define ESP_NOW_CHANNEL 1

#define PIN_JS_LX  39
#define PIN_JS_LY  36
#define PIN_JS_LSW 15

#define PIN_JS_MX  35
#define PIN_JS_MY  34
#define PIN_JS_MSW 16

#define PIN_JS_RX  33
#define PIN_JS_RY  32
#define PIN_JS_RSW 4

#define PIN_I2C_SDA 21
#define PIN_I2C_SCL 22

#define MIN_JOYSTICK     0
#define MAX_JOYSTICK     4095

#define JS_NORM_CENTER   1800
#define JS_NORM_NRANGE   1800
#define JS_NORM_PRANGE   2200
#define JS_NORM_DEADZONE 100

#define MIN_TRANSMITTER -100
#define MID_TRANSMITTER  0 
#define MAX_TRANSMITTER  100

//esp32 core 0
#define ESP_CORE_0 0 
//esp32 core 1
#define ESP_CORE_1 1 
//freertos task stack size
#define TASK_STACK_SIZE 2048*2
//freertos task delay microseconds (periodic)
#define TASK_DELAY_MS 10

#endif