#include <stdio.h>
#include <esp_err.h>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/ledc.h>
#include "PWM.h"

static esp_err_t ret;
uint8_t duty_res = 1;

void PWM_Init_Start(uint32_t PWM_freq)
{
    static const char* TAG = "PWM_Init";

    ledc_timer_config_t ledc_timer_cfg = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = duty_res, //ledc_calculate_divisor
        .timer_num = LEDC_TIMER_0,
        .freq_hz = PWM_freq,
        .clk_cfg = LEDC_USE_APB_CLK
    };
    while (true)
    {
        ret = ledc_timer_config(&ledc_timer_cfg);
        if (ret == ESP_OK) break;
        ++ledc_timer_cfg.duty_resolution;
        duty_res = ledc_timer_cfg.duty_resolution;
        if (duty_res > SOC_LEDC_TIMER_BIT_WIDE_NUM)abort(); //can't imagine
    }
    ESP_LOGI(TAG, "ledc timer config done, freq:%lu, final res:%u", PWM_freq, duty_res);

    ledc_channel_config_t ledc_channel_cfg = {
        .gpio_num       = 27,
        .speed_mode     = LEDC_HIGH_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .duty           = (1 << (duty_res - 1)),
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_cfg));
    ESP_LOGI(TAG, "ledc channel config done");
}
