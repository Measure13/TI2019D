#pragma once

#define USING_AD9833 true

void SPI_Init(void);
void SPI_Transmit(uint16_t content);

void AD9833_Reset(void);
void AD9833_Set_Freq(uint32_t output_freq, uint32_t ref_freq);
void AD9833_Set_Phase(uint16_t init_phase);
void AD9833_Set(void);
void AD9833_Default_Set(uint32_t output_freq, uint32_t ref_freq);