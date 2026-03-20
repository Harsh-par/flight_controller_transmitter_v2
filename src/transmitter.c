#include "transmitter.h"

void transmitter_init(controller_data_t* transmitter){
    transmitter->joystick_lx = 0;
    transmitter->joystick_ly = 0;
    transmitter->button_l    = 0;

    transmitter->joystick_mx = 0;
    transmitter->joystick_my = 0;
    transmitter->button_m    = 0;

    transmitter->joystick_rx = 0;
    transmitter->joystick_ry = 0;
    transmitter->button_r    = 0;

    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();

    wifi_init_config_t wifi_configuration = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_configuration);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_set_channel(ESP_NOW_CHANNEL, WIFI_SECOND_CHAN_NONE);

    esp_now_init();
    esp_now_register_send_cb(transmitter_callback);
    
    esp_now_peer_info_t esp_now_peer = {0};

    memcpy(esp_now_peer.peer_addr, rx_mac_address, sizeof(rx_mac_address));

    esp_now_peer.ifidx   = WIFI_IF_STA;
    esp_now_peer.channel = ESP_NOW_CHANNEL;
    esp_now_peer.encrypt = false;

    esp_now_add_peer(&esp_now_peer);

    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
}

void transmitter_set(controller_data_t* transmitter, joystick_t* joystick_l, joystick_t* joystick_m, joystick_t* joystick_r){
    transmitter->joystick_lx = joystick_normalize(joystick_l->x, JS_NORM_CENTER, JS_NORM_NRANGE, JS_NORM_PRANGE, JS_NORM_DEADZONE);
    transmitter->joystick_ly = joystick_normalize(joystick_l->y, JS_NORM_CENTER, JS_NORM_NRANGE, JS_NORM_PRANGE, JS_NORM_DEADZONE);
    
    transmitter->joystick_mx = joystick_normalize(joystick_m->x, JS_NORM_CENTER, JS_NORM_NRANGE, JS_NORM_PRANGE, JS_NORM_DEADZONE);
    transmitter->joystick_my = joystick_normalize(joystick_m->y, JS_NORM_CENTER, JS_NORM_NRANGE, JS_NORM_PRANGE, JS_NORM_DEADZONE);

    transmitter->joystick_rx = joystick_normalize(joystick_r->x, JS_NORM_CENTER, JS_NORM_NRANGE, JS_NORM_PRANGE, JS_NORM_DEADZONE);
    transmitter->joystick_ry = joystick_normalize(joystick_r->y, JS_NORM_CENTER, JS_NORM_NRANGE, JS_NORM_PRANGE, JS_NORM_DEADZONE);

    transmitter->button_l = joystick_l->state;
    transmitter->button_m = joystick_m->state;
    transmitter->button_r = joystick_r->state;
}

void transmitter_send(controller_data_t* transmitter){
    esp_now_send(rx_mac_address, (uint8_t*)transmitter, sizeof(controller_data_t));
}

void transmitter_callback(const esp_now_send_info_t* tx_info, esp_now_send_status_t status){
    ESP_LOGI("tx espnow", "%s", status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
    
    if(status == ESP_NOW_SEND_SUCCESS) tx_connected_to_rx = true;
    else tx_connected_to_rx = false;
    
    gpio_set_level(GPIO_NUM_2, !status);
}