#include "rfid.h"

static bool add_mode = false;

static char *TAG = "RFID";

int add = 0;

static void rc522_handler(void* arg, esp_event_base_t base, int32_t event_id, void* event_data)
{
    rc522_event_data_t* data = (rc522_event_data_t*) event_data;

    switch(event_id) {
        case RC522_EVENT_TAG_SCANNED: {
                rc522_tag_t* tag = (rc522_tag_t*) data->ptr;
                ESP_LOGI(TAG, "Tag scanned (sn: %" PRIu64 ")", tag->serial_number);
                if (!add) {
                    add_id(tag->serial_number);
                    add++;
                }
                else if (check_id(tag->serial_number)) {
                    ESP_LOGI(TAG,"ID Found!");
                    servo_set_angle(90);
                }
                else {
                    ESP_LOGI(TAG,"New ID!");
                }

                // if ()
                //     add_id(tag->serial_number);
            }
            break;
    }
}



static void printJsonObject(cJSON *item)
{
        char *json_string = cJSON_Print(item);
        if (json_string) 
        {
            ESP_LOGI("JSON","%s\n", json_string);
            cJSON_free(json_string);
        }
}


bool check_id(uint64_t number) {
    FILE *json = fopen("/spiffs/id.txt","r+");
    if (json == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return false;
    }
    char file_buff[100];
    fgets(file_buff,sizeof(file_buff),json);

    cJSON *id_buffer = cJSON_Parse(file_buff);

    fclose(json);
    printJsonObject(id_buffer);
    cJSON *size = cJSON_GetObjectItem(id_buffer,"size");
    int size_val = 0;
    if (size) {
        size_val = size->valueint;
    }
    ESP_LOGI(TAG, "size: %d",size_val);

    int i;
    char idx[20];
    for (i = 1;i <= size_val;i++) {
        sprintf(idx,"%d",i);
        
        ESP_LOGI("Idx","%s",idx);
        cJSON* val = cJSON_GetObjectItem(id_buffer,idx);
        uint64_t valID = 0;
        if(val) {
            valID = (uint64_t)val->valuedouble;
            ESP_LOGI(TAG,"valID: %" PRIu64" ",valID);
        }
        if (number == valID) {
            return true;
        }
        cJSON_free(val);
    }

    return false;
}


void spiffs_config() {
    ESP_LOGI(TAG, "Initializing SPIFFS");
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    ESP_LOGI(TAG, "Opening file");
    FILE* f = fopen("/spiffs/id.txt", "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root,"size",0);
    printJsonObject(root);
    fprintf(f,cJSON_PrintUnformatted(root));
    fclose(f);

    ESP_LOGI(TAG, "File created");
}




void rfid_config() {
    rc522_config_t config = {
        .spi.host = VSPI_HOST,
        .spi.miso_gpio = 19,
        .spi.mosi_gpio = 23,
        .spi.sck_gpio = 18,
        .spi.sda_gpio = 5,
    };

    rc522_create(&config, &scanner);
    rc522_register_events(scanner, RC522_EVENT_ANY, rc522_handler, NULL);
    rc522_start(scanner);
    servo_set_angle(0);
}

void add_id(uint64_t number){
    FILE *f = fopen("/spiffs/id.txt", "r+");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    char file_buff[100];
    fgets(file_buff,sizeof(file_buff),f);
    fclose(f);
    ESP_LOGI(TAG, "File buffer: %s",file_buff);
    cJSON *json = cJSON_Parse(file_buff);
    printJsonObject(json);
    cJSON *size = cJSON_GetObjectItem(json,"size");
    int size_val = 0;
    if (size) {
        size_val = size->valueint;
    }
    ESP_LOGI(TAG, "size: %d",size_val);
    size_val++;
    char key_id[20];

    sprintf(key_id,"%d",size_val);

    cJSON_AddNumberToObject(json,key_id,number);

    cJSON_SetNumberValue(cJSON_GetObjectItem(json,"size"),size_val);

    printJsonObject(json);

    f = fopen("/spiffs/id.txt", "w");

    char *data_buff = cJSON_PrintUnformatted(json);
    fprintf(f,data_buff);
    cJSON_free(json);
    fclose(f);
}