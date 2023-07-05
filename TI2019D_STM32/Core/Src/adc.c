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
#define PI 3.14159265
uint32_t adc_freq = 0;
static double complex output[MAX_DATA_NUM];
uint16_t adc_values[MAX_DATA_NUM + 4];
uint16_t adc_values_cnt = 0;
uint8_t adc_data_owner = INPUT_RESISTANCE;
static bool conv_done = false;
static bool first = true;

double Ri = 0;
double Ro = 0;
float Gain[240];

static double ADC_Get_FFTVpp(double* data, double complex *output, int f, int N);

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
  sConfig.Channel = ADC_CHANNEL_0;
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
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
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
    HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
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
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);

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
	uint32_t sum = 0;
	uint32_t v0, v1;
	
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = channel % 2;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	do {
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // * start ADC
		while (!conv_done)
		{
			
		}
		conv_done = false;
		for (int i = 4; i < MAX_DATA_NUM + 4; ++i)
		{
			sum += adc_values[i];
		}
		if (first)
		{
			v0 = sum / MAX_DATA_NUM;
		}
		else
		{
			v1 = sum / MAX_DATA_NUM;
			if (adc_data_owner == INPUT_RESISTANCE)
			{
				Ri = 6800 / (v0 / v1 - 1);
				DAC_Output(Ri / 1000);
			}
			else
			{
				Ro = 1500 * (v0 / v1 - 1);
				DAC_Output(Ro / 1000);
			}
		}
		first = ~first;
		sum = 0;
	}while (!first);
}

double ADC_Get_Gain(int channel)
{
	double FFTVpp0, FFTVpp1, Gain;
	double temp[MAX_DATA_NUM];

	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	do {
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // * start ADC
		while (!conv_done)
		{
			
		}
		conv_done = false;
		for (int i = 4; i < MAX_DATA_NUM + 4; ++i)
		{
			temp[i - 4] = (double)adc_values[i];
		}
		if (first)
			FFTVpp0 = ADC_Get_FFTVpp(temp, output, DDS_Freq, MAX_DATA_NUM);

			sConfig.Channel = ADC_CHANNEL_1;
			sConfig.Rank = 1;
			sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
			if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
			{
				Error_Handler();
			}
		else
		{
			FFTVpp1 = ADC_Get_FFTVpp(temp, output, DDS_Freq, MAX_DATA_NUM);
			Gain = FFTVpp1 / FFTVpp0;
		}
		first = ~first;
	}while (!first);
	return Gain;
}

static void hammingWindow(double* frame,int FRAME_LEN) {
    for (int i = 0; i < FRAME_LEN; i++) {
        frame[i] *= 0.54 - 0.46 * cos(2 * PI * i / FRAME_LEN);
    }
}

static void fft(double complex *x, int N, double complex *X)
{
    if (N == 1) {
        *X = *x;
        return;
    }

    double complex odd[N / 2];
    double complex even[N / 2];

    for (int i = 0; i < N / 2; i++) {
        even[i] = x[2 * i];
        odd[i] = x[2 * i + 1];
    }

    double complex Even[N / 2];
    double complex Odd[N / 2];
    fft(even, N / 2, Even);
    fft(odd, N / 2, Odd);

    for (int k = 0; k < N / 2; ++k) {
        double complex twiddle_factor = cexp(-2 * PI * I * k / N);
        X[k] = Even[k] + twiddle_factor * Odd[k];
        X[k + N / 2] = Even[k] - twiddle_factor * Odd[k];
    }
}

static void rfft(double *input, int N, double complex *output)
{
    double complex x[2*N];
    for (int i = 0; i < N; i++) {
        if (i < N / 2) {
            x[i] = input[2 * i];
        } else {
            x[i] = input[2 * (N - i - 1)];
        }
        x[i + 1] = 0;
    }

    fft(x, N, output);
}

static double ADC_Get_FFTVpp(double* data, double complex *output, int f, int N)
{
    if(f < 100000){
        double max = data[0];
        double min = data[0];
        for(int i = 0; i < N; ++i){
            if(data[i] > max) max = data[i];
            if(data[i] < min) min = data[i];
        }
        return (max - min);
    }
    else{
        hammingWindow(data,1024);
        rfft(data, N, output);
        double FFTVpp0 = 0,FFTVpp = 0;
        double A ;
        int P;
        for(int i = 10; i < N / 2; ++i){
            A = sqrt(creal(output[i]) * creal(output[i]) + cimag(output[i]) * cimag(output[i]));
            if(A > FFTVpp0) {
                FFTVpp0 = A;
                P = i;
            }
        }
        for(int i = P-20; i < P+20; ++i){
            A = sqrt(creal(output[i]) * creal(output[i]) + cimag(output[i]) * cimag(output[i]));
            if(A > FFTVpp0 / 20){
                FFTVpp = sqrt(FFTVpp * FFTVpp + A * A);
            }
        }
        return FFTVpp;
    }
}

uint16_t* ADC_Pointer_With_Data(int channel)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = channel % 2;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	DAC_Output(0.5);
	memset(adc_values, 0x00, sizeof(uint16_t) * (MAX_DATA_NUM + 4));
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // * start ADC
	DAC_Output(0.7);
	while (!conv_done)
	{
		
	}
	conv_done = false;
	DAC_Output(0.9);
	return adc_values;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(hadc);
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
    DAC_Output(2.9);
	conv_done = true;
}
/* USER CODE END 1 */
