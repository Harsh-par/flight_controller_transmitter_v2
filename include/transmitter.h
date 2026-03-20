#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <stdint.h>

#include "driver/gpio.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"

#include "constants.h"
#include "globals.h"
#include "joystick.h"

// this is where the receiver esp32's mac address would go
static const uint8_t rx_mac_address[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

typedef struct{
    int16_t joystick_lx, joystick_ly, button_l;
    int16_t joystick_mx, joystick_my, button_m;
    int16_t joystick_rx, joystick_ry, button_r;
} controller_data_t;

void transmitter_init(controller_data_t* transmitter);
void transmitter_set(controller_data_t* transmitter, joystick_t* joystick_l, joystick_t* joystick_m, joystick_t* joystick_r);
void transmitter_send(controller_data_t* transmitter);
void transmitter_callback(const esp_now_send_info_t* tx_info, esp_now_send_status_t status);

#endif