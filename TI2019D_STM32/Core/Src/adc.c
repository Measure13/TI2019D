/* USER CODE BEGIN Header */
/**
	******************************************************************************
	* @file    adc.c
	* @brief   This file provides code for the configuration
	*          of the ADC instances.
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
/* Includes ------------------------------------------------------------------*/
#include "adc.h"

/* USER CODE BEGIN 0 */
#include<math.h>
#include<complex.h>
#define PI  3.14159265
#define OP  50
uint32_t adc_freq = 0;
//static float complex output[MAX_DATA_NUM];
static float temp[MAX_DATA_NUM];
// static float complex x[2*MAX_DATA_NUM];
uint16_t adc_values[MAX_DATA_NUM + 4];
uint16_t adc_values_cnt = 0;
uint8_t adc_data_owner = INPUT_RESISTANCE;
static volatile bool conv_done = false;
static bool first = true;

float Ri = 0;
float Ro = 0;
//float Gain[240];

// static float ADC_Get_FFTVpp(float* data, float complex *output, int f, int N);
static float ADC_Get_Vpp(float* data);

/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

/* ADC1 init function */
void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */
	
  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_CC2;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */
	
  /* USER CODE END ADC1_Init 2 */

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PA0-WKUP     ------> ADC1_IN0
    PA1     ------> ADC1_IN1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ADC1 DMA Init */
    /* ADC1 Init */
    hdma_adc1.Instance = DMA2_Stream0;
    hdma_adc1.Init.Channel = DMA_CHANNEL_0;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc1);

    /* ADC1 interrupt Init */
    HAL_NVIC_SetPriority(ADC_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(ADC_IRQn);
  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PA0-WKUP     ------> ADC1_IN0
    PA1     ------> ADC1_IN1
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1);

    /* ADC1 DMA DeInit */
    HAL_DMA_DeInit(adcHandle->DMA_Handle);

    /* ADC1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(ADC_IRQn);
  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void ADC_Get_Resistance(int channel)
{
	float v0, v1;
	
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = channel % 2;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
    SW2_DISC;
    SW3_CONN;
	do {
		if (first)
		{
            // 实际上这个连线是一样的， 第二次也是
            if (adc_data_owner == INPUT_RESISTANCE)
            {
                SW0_CONN; // 直接接受Vin
                SW1_DISC; // 直接接受Vout
            }
            else if (adc_data_owner == OUTPUT_RESISTANCE)
            {
                SW0_CONN;
                SW1_DISC;
            }
            HAL_Delay(RELAY_DELAY);
			Timer_2_Adjust(1000000);
			memset(adc_values, 0x0000, sizeof(uint16_t) * (MAX_DATA_NUM + 4));
            HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // * start ADC
            while (!conv_done)
            {
                
            }
            conv_done = false;
            for (int i = 4; i < MAX_DATA_NUM + 4; ++i)
            {
                temp[i - 4] = (float)adc_values[i];
            }
            v0 = ADC_Get_Vpp(temp);
		}
		else
		{
            if (adc_data_owner == INPUT_RESISTANCE)
            {
                SW0_DISC; // 被前端输出电阻拉低了
                SW1_DISC; // 被后端输入电阻拉低了
            }
            else if (adc_data_owner == OUTPUT_RESISTANCE)
            {
                SW0_CONN;
                SW1_CONN;
            }
            HAL_Delay(RELAY_DELAY);
			Timer_2_Adjust(1000000);
			memset(adc_values, 0x0000, sizeof(uint16_t) * (MAX_DATA_NUM + 4));
            HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // * start ADC
            while (!conv_done)
            {
                
            }
            conv_done = false;
            for (int i = 4; i < MAX_DATA_NUM + 4; ++i)
            {
                temp[i - 4] = (float)adc_values[i];
            }
            v1 = ADC_Get_Vpp(temp);
			if (adc_data_owner == INPUT_RESISTANCE)
			{
				Ri = 6800.0f / (v0 / v1 - 1.0f);
				UARTHMI_Send_Float(0, Ri / 1000.0f); //kilo om
			}
			else
			{
				Ro = 1500.0f * (v0 / v1 - 1.0f);
				UARTHMI_Send_Float(1, Ro / 1000.0f);
			}
		}
		first = (first == true);
	}while (!first);
}

float ADC_Get_Gain()
{
	float FFTVpp0, FFTVpp1, Gain;

	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	do {
		if (DDS_Freq < 1000)
		{
			Timer_2_Adjust(100000);
		}
		else
		{
			Timer_2_Adjust(1000000);
		}
		memset(adc_values, 0x0000, sizeof(uint16_t) * (MAX_DATA_NUM + 4));
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
		while (!conv_done)
		{
			
		}
		conv_done = false;
		for (int i = 4; i < MAX_DATA_NUM + 4; ++i)
		{
			temp[i - 4] = (float)adc_values[i];
		}
		if (first)
        {
            FFTVpp0 = ADC_Get_Vpp(temp) / 100.0f; //* The input signal is amplified 100 times
            // prepare for the other one
			sConfig.Channel = ADC_CHANNEL_1;
			sConfig.Rank = 1;
			sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
			if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
			{
				Error_Handler();
			}
        }
		else
		{
            FFTVpp1 = ADC_Get_Vpp(temp);
			Gain = FFTVpp1 / FFTVpp0;
		}
		first = (first == false);
	}while (!first);
	return Gain;
}

static float ADC_Get_Vpp(float* data)
{
    if(DDS_Freq < 100000){
        float max = data[0];
        float min = data[0];
        for(int i = 0; i < MAX_DATA_NUM; ++i){
            if(data[i] > max) max = data[i];
            if(data[i] < min) min = data[i];
        }
        return (max - min);
    }
    else
    {
        float cos_theta = cosf(2 * PI * DDS_Freq / 1000000), sin_theta = sinf(2 * PI * DDS_Freq / 1000000);
        float DC = 0, A = 0;
        for(int i = 0; i < OP; ++i){
            DC += data[i];
        }
        DC /= OP;
        for (int i = 0; i < OP; i += 2)
        {
            A += sqrtf(powf((data[i + 1] - DC - cos_theta * (data[i] - DC)) / sin_theta, 2.0f) + (data[i] - DC) * (data[i] - DC));
        }
        A /= OP;
        return A * 2;
    }
    
}

//! Note the channel! Only 0 and 1 is reachable!
float ADC_Get_DC_Voltage(int channel)
{
    uint16_t max;
    uint16_t min;
    float Voltage = 0;
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = channel % 2;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	memset(adc_values, 0x00, sizeof(uint16_t) * (MAX_DATA_NUM + 4));
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // * start ADC
	while (!conv_done)
	{
		
	}
	conv_done = false;
    max = adc_values[4];
    min = adc_values[4];
    for(int i = 5; i < MAX_DATA_NUM + 4; ++i){
        if(adc_values[i] > max) max = adc_values[i];
        if(adc_values[i] < min) min = adc_values[i];
        Voltage += (float)adc_values[i];
    }
    Voltage /= MAX_DATA_NUM;
    Voltage += (float)(max + min);
    
	return Voltage / 4096.0f * 1.1f; // 3.3 / 3 = 1.1
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(hadc);
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
	conv_done = true;
}
/* USER CODE END 1 */
