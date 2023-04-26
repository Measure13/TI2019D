#include <stdio.h>
#include <esp_log.h>
#include <esp_err.h>
// #include "PWM.h"
#include "SPI_AD9833.h"
#include "Rainmaker.h"

static const char* TAG = "main";

static uint32_t ref_freq;
static uint32_t output_freq;

void app_main(void)
{
    ESP_LOGI(TAG, "Start!");
    ref_freq = 25 *1000 * 1000;
    output_freq = 4000;
    SPI_Init();
    AD9833_Default_Set(output_freq, ref_freq);

    // Rainmaker_Init();
}
