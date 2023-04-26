#include <stdio.h>
#include <esp_log.h>
#include <esp_err.h>
// #include "PWM.h"
#include "SPI_AD9833.h"
#include "Rainmaker.h"

static const char* TAG = "main";

static uint32_t output_freq;

void app_main(void)
{
    ESP_LOGI(TAG, "Start!");
    output_freq = 900;
    SPI_Init();
    AD9833_Default_Set(output_freq);

    Rainmaker_Init();
}
