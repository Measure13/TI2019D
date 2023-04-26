#include <stdio.h>
#include <string.h>
#include <esp_log.h>
#include <esp_err.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "SPI_AD9833.h"
#include "SPI_Hardware.h"

void SPI_Init(void)
{
    gpio_config_t gpio_cfg = {
        .pin_bit_mask = BITS_MASK,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&gpio_cfg);

    CS_1();// CS_0();
    FSYNC_1();
    SCLK_0();
}

void SPI_Transmit(uint16_t content)
{
    ESP_LOGI("HSPI", "ctt:%x", content);
    SCLK_1();
    FSYNC_1();
    FSYNC_0();
    CS_0();
    for(int i = 0; i < 16; i++)
	{
        SCLK_1();
		if (content & 0x8000)
			MOSI_1();
		else
			MOSI_0();
		
		AD9833_Delay();
		SCLK_0();
		AD9833_Delay();
        content <<= 1;
	}
    FSYNC_1();
    CS_1();
}