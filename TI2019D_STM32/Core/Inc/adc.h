/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.h
  * @brief   This file contains all the function prototypes for
  *          the adc.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "usart.h"
#include "tim.h"
#include "dac.h"
#include <stdbool.h>
/* USER CODE END Includes */

extern ADC_HandleTypeDef hadc1;

/* USER CODE BEGIN Private defines */
#define INPUT_RESISTANCE  0
#define OUTPUT_RESISTANCE 1
#define Amp_Freq_Curv     2
#define INF_RI            100000.0f //100k

extern uint32_t adc_freq;
extern uint16_t adc_values[MAX_DATA_NUM + 4];
extern uint8_t adc_data_owner;
extern float Gain[240];
extern float Ri;
extern float Ro;

/* USER CODE END Private defines */

void MX_ADC1_Init(void);

/* USER CODE BEGIN Prototypes */
void ADC_Warm_Up(void);
float median(float* data, uint8_t len, bool flag);
float ADC_Get_Gain(void);
void ADC_Get_Resistance(int channel);
float ADC_Get_DC_Voltage(int channel);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */

