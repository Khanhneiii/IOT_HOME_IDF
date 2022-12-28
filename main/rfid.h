#ifndef _RFID_H

#define _RFID_H

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "cJSON.h"
#include "rc522.h"
#include "servo_ctrl.h"

static rc522_handle_t scanner;


bool check_id(uint64_t);

void spiffs_config();

void rfid_config();

void add_id(uint64_t);

#endif