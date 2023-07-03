/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "adc.h"
#include "tim.h"
#include <stdio.h>
#include <string.h>
#define UART_RX_BUF_SIZE 2048 // 1024 * sizeof(uint16_t)
// extern uint32_t adc_freq;

static uint8_t uart1_rx_bp[UART_RX_BUF_SIZE];
static uint8_t uart1_tx_bp[UART_RX_BUF_SIZE];
static uint8_t uart1_rx_cnt = 0;
static uint8_t uart1_tx_cnt = 0;
static uint8_t uart1_rx_buf = 0;

static const int batch = 16;
/* USER CODE END 0 */

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
  HAL_UART_Receive_IT(&huart1, (uint8_t *)&uart1_rx_buf, 1);
  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA2_Stream7;
    hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
int fputc(int ch, FILE *stream)
{
	while((USART1->SR & 0X40) == 0);

	USART1->DR = (uint8_t) ch;

	return ch;
}

// void UART_RX_Data_Parse(uint8_t* p, uint8_t cnt)
// {
// 	adc_freq = 0;
// 	for (int i = 0; i < 4; ++i)
// 	{
// 		adc_freq += ((p[i] & 0xff) << (8 * i));
// 	}
// 	memset(uart1_tx_bp, 0x00, sizeof(uint8_t) * UART_RX_BUF_SIZE);
// 	// printf("%d\n", adc_freq);
// 	Timer_2_Adjust(adc_freq);
// }

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(huart);
 
	if(uart1_rx_cnt >= UART_RX_BUF_SIZE)
	{
		uart1_rx_cnt = 0;
		memset(uart1_rx_bp, 0x00, sizeof(uart1_rx_bp));
		USART_Send_Data_Direct("Overflow!!!\n", 13); 	
	}
	else
	{
		uart1_rx_bp[uart1_rx_cnt++] = uart1_rx_buf;
	
		if((uart1_rx_cnt > 3)&&(uart1_rx_bp[uart1_rx_cnt-3] == 0xFF)&&(uart1_rx_bp[uart1_rx_cnt-2] == 0xFF)&&(uart1_rx_bp[uart1_rx_cnt-1] == 0xFF))
		{
			// UART_RX_Data_Parse(uart1_rx_bp, uart1_rx_cnt);
			uart1_rx_cnt = 0;
			memset(uart1_rx_bp, 0x00, sizeof(uint8_t) * UART_RX_BUF_SIZE);
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
		}
	}
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&uart1_rx_buf, 1);
}

void USART_Conv_Data(uint16_t* adc_data_p, uint16_t length)
{
	uart1_tx_cnt = length * 2;
	for (int i = 0; i < batch; ++i)
	{
		//! Experimental facts shows that transmit size should be no greater than 128
		HAL_UART_Transmit(&huart1, (uint8_t*)adc_data_p + i * UART_RX_BUF_SIZE / batch, UART_RX_BUF_SIZE / batch, 0xFFFFFFFF);
		while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);
	}
	memset(adc_data_p, 0x00, sizeof(uint16_t) * length);
}

void USART_Send_Data_Direct(uint8_t* data_p, uint16_t data_len)
{
	HAL_UART_Transmit(&huart1, data_p, data_len, 0xFFFF);
	while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);
}

void USART_Send_Data_Temp(uint8_t* data_p, uint16_t data_len)
{
	strncpy((char*)uart1_tx_bp, (char*)data_p, data_len);
	HAL_UART_Transmit(&huart1, uart1_tx_bp, uart1_tx_cnt, 0xFFFF);
	while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);
	memset(uart1_tx_bp, 0x00, sizeof(uint8_t) * UART_RX_BUF_SIZE);
}
/* USER CODE END 1 */
