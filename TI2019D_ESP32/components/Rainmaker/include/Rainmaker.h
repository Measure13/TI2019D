/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <esp_rmaker_core.h>

#define DEFAULT_POWER  true
#define DEVICE_NAME     "Test"
#define DEVICE_TYPE     "esp.device.test"

typedef struct
{
   bool power_state;
   uint32_t freq_fine;
   char* freq_coarse;
}app_driver_info;

extern esp_rmaker_device_t *rainmaker_device;
extern const char * freq_units[];
extern app_driver_info driver_info;
extern QueueHandle_t xQueue;

void app_driver_init(void);
void app_driver_set_state(app_driver_info di);
app_driver_info app_driver_get_state(void);
void Rainmaker_Init(void);
void app_driver_SPI(void *pvParameter);