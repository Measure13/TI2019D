#include "Rainmaker.h"
#include "SPI_AD9833.h"
#include <esp_log.h>
#include <string.h>

#define APP_SPI_TAG "app SPI"

app_driver_info driver_info;

void app_driver_init(void)
{
    driver_info.power_state = DEFAULT_POWER;
    driver_info.freq_fine = 400;
    driver_info.freq_coarse = "Hz";
}
void app_driver_set_state(app_driver_info di)
{
    ESP_LOGI("driver", "set");
}
app_driver_info app_driver_get_state(void)
{
    ESP_LOGI("driver", "get");
    return driver_info;
}

void app_driver_SPI(void *pvParameter)
{
    uint32_t factor = 1;
    while (1)
    {
        ESP_LOGI(APP_SPI_TAG, "waiting...");
        xQueueReceive(xQueue, &(driver_info.power_state), portMAX_DELAY);
        ESP_LOGI(APP_SPI_TAG, "get one task!");
        for (int i = 0; i < 3; ++i)
        {
            if (strcmp(freq_units[i], driver_info.freq_coarse) == 0)
            {
                AD9833_Default_Set(driver_info.freq_fine * factor);
                break;
            }
            factor *= 1000;
        }
        driver_info.power_state = false;
        xQueueSend(xQueue, &(driver_info.power_state), portMAX_DELAY);
        vTaskDelete(NULL);
    }
}