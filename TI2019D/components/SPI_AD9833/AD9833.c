#include <stdio.h>
#include <esp_log.h>
#include <esp_err.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/spi_master.h>
#include <driver/spi_common.h>
#include <driver/gpio.h>
#include "SPI_AD9833.h"
#include "AD9833.h"

static uint16_t isc = 0x0000;//instruction
static const char* TAG = "AD9833";

void AD9833_Reset(void)
{
    ZERO(isc);
    AD_CONT_RST(isc);
    SET(isc, AD_CONT_B28); // send 28 bits in 2 runs
    SPI_Transmit(isc);
}

void AD9833_Set_Freq(uint32_t output_freq)
{
    ZERO(isc);
    uint32_t delta_phase = (uint32_t)((double)output_freq * phase_all / ref_freq + 0.5);
    // ESP_LOGI(TAG, "delta phase:%lu", delta_phase); // less than 268435455, or 0xfff ffff
    isc = (uint16_t)(delta_phase & 0x00003fff); // LSB 14
    AD_FREQ_1(isc);
    SPI_Transmit(isc);
    isc = (uint16_t)((delta_phase & 0x0fffc000) >> 14); // MSB 14
    AD_FREQ_1(isc);
    SPI_Transmit(isc);
}

void AD9833_Set_Phase(uint16_t init_phase)
{
    AD_PHAS_1(init_phase);
    SPI_Transmit(init_phase);
}

void AD9833_Set(void)
{
    ZERO(isc);
    AD_CONT_SET(isc);
    SET(isc, AD_CONT_B28);
    SPI_Transmit(isc);
}

void AD9833_Default_Set(uint32_t output_freq)
{
    AD9833_Reset();
    AD9833_Set_Freq(output_freq);
    AD9833_Set_Phase(0);
    AD9833_Set();
}
