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

    CS_1();
    FSYNC_1();
    SCLK_0();
}

void SPI_Transmit(uint16_t* content)
{
    uint16_t mask = 1;
    SCLK_1();
    FSYNC_0();
    CS_0();
    for (int i = 15; i >= 0;--i)
    {
        SCLK_1();
        if (((0x01 << mask) & (*content)))MOSI_1();
        else MOSI_0();
        SCLK_0();
    }
    FSYNC_1();
    CS_1();
}