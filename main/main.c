#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <esp_log.h>
#include <string.h>
#include "rfid.h"
#include "mqtt_ctrl.h"
#include "wifi.h"
#include "dht11.h"
#include "alarm.h"
#include  "WTV020SD16P.h"

#define SERVO_PIN 27
#define GAS_PIN 12
#define PIR_PIN 14
#define MAIN "ESP"

void app_main(void){

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(MAIN, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    DHT11_init(4);

    InitAlarm(GAS_PIN,PIR_PIN);

    init_servo(15);

    spiffs_config();

    rfid_config();      

    mqtt_app_start();
}