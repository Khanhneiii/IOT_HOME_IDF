#include "alarm.h"
#include "esp_log.h"

static const char* TAG = "ALARM";

static int GAS_PIN,PIR_PIN;

void InitAlarm(int gas,int pir) {
    GAS_PIN = gas;
    PIR_PIN = pir;
    gpio_config_t io = {
        .pin_bit_mask = (1ULL<<GAS_PIN) | (1ULL<<PIR_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 0,                           
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&io);
}

void AlarmTask(void *pvParameters) {

    while (1) {
        if (gpio_get_level(GAS_PIN) == 0){
            ESP_LOGI(TAG,"GAS!");
        }
        if (gpio_get_level(GAS_PIN) == 1) {
            ESP_LOGI(TAG,"Motion!");
        }
    }
}