#ifndef __RELAY_H
#define __RELAY_H

#include "gpio.h"

#define SW0_ON      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET)
#define SW0_OFF     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET)
#define SW1_ON      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET)
#define SW1_OFF     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET)
#define SW2_ON      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET)
#define SW2_OFF     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET)
#define SW3_ON      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
#define SW3_OFF     HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);

#endif