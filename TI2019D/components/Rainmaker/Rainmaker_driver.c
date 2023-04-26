#include "Rainmaker.h"
#include <esp_log.h>

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