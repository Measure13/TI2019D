/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "AD9833.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t DDS_Freq;
uint32_t DDS_Freq_Array[] = {100, 103, 107, 110, 114, 117, 121, 125, 129, 133, 137, 142, 146, 151, 156, 161, 166, 171, 177, 183, 188, 194, 201, 207, 214, 221, 228, 235, 243, 251, 259, 267, 276, 284, 294, 303, 313, 323, 333, 344, 355, 366, 378, 390, 403, 416, 429, 443, 457, 472, 487, 503, 519, 536, 553, 571, 589, 608, 628, 648, 669, 690, 712, 735, 759, 783, 809, 835, 862, 889, 918, 947, 978, 1009, 1042, 1075, 1110, 1146, 1183, 1221, 1260, 1300, 1342, 1385, 1430, 1476, 1524, 1573, 1623, 1675, 1729, 1785, 1842, 1902, 1963, 2026, 2091, 2159, 2228, 2300, 2374, 2450, 2529, 2610, 2694, 2781, 2870, 2963, 3058, 3157, 3258, 3363, 3471, 3583, 3698, 3817, 3940, 4067, 4198, 4333, 4472, 4616, 4765, 4918, 5076, 5239, 5408, 5582, 5762, 5947, 6138, 6336, 6540, 6750, 6967, 7192, 7423, 7662, 7908, 8163, 8426, 8697, 8977, 9265, 9564, 9871, 10189, 10517, 10855, 11204, 11565, 11937, 12321, 12718, 13127, 13549, 13985, 14435, 14900, 15379, 15874, 16385, 16912, 17456, 18018, 18598, 19196, 19814, 20451, 21109, 21789, 22490, 23213, 23960, 24731, 25527, 26348, 27196, 28071, 28975, 29907, 30869, 31863, 32888, 33946, 35038, 36166, 37330, 38531, 39771, 41050, 42371, 43734, 45142, 46594, 48094, 49641, 51238, 52887, 54589, 56345, 58158, 60030, 61961, 63955, 66013, 68137, 70330, 72593, 74928, 77339, 79828, 82397, 85048, 87785, 90609, 93525, 96534, 99640, 102847, 106156, 109572, 113097, 116737, 120493, 124370, 128372, 132503, 136766, 141167, 145709, 150398, 155237, 160232, 165388, 170710, 176203, 181873, 187725, 193765};
float Gain_Array[MAX_SEND_LEN];
static float A_mf, Output_DC;
static int freq_upper = 160000, Component = 0, Fault = 0;
static bool already = true;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Find_Fault_R(void){//Ri为输入电阻，V为直流电压的平均值
    if(Output_DC > 11.5f  && Ri > 12000.0f)                       {Component = R1; Fault = OPEN_CIRCUIT;} //R1断
    else if(Output_DC > 11.0f  && Output_DC < 11.5f)              {Component = R1; Fault = SHORT_CIRCUIT;}//R1短
    else if(Output_DC > 3.5f   && Output_DC < 5.0f)               {Component = R2; Fault = OPEN_CIRCUIT;} //R2断
    else if(Output_DC > 11.5f  && Ri < 1000.0f)                   {Component = R2; Fault = SHORT_CIRCUIT;}//R2短
    else if(Output_DC > 0.1f   && Output_DC < 1.0f)               {Component = R3; Fault = OPEN_CIRCUIT;} //R3断
    else if(Output_DC > 11.5f  && Ri > 1000.0f && Ri < 3000.0f)   {Component = R3; Fault = SHORT_CIRCUIT;}//R3短
    else if(Output_DC > 11.5f  && Ri > 10000.0f && Ri < 12000.0f) {Component = R4; Fault = OPEN_CIRCUIT;} //R4断
    else if(Output_DC < 0.1f)                                     {Component = R4; Fault = SHORT_CIRCUIT;}//R4短
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_DAC_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_values, MAX_DATA_NUM + 4);
  UARTHMI_Forget_It();
	
  //* start
  DDS_Freq = 1000;
  AD9833_Default_Set(DDS_Freq);
  adc_data_owner = INPUT_RESISTANCE;
  ADC_Get_Resistance(adc_data_owner);
  adc_data_owner = OUTPUT_RESISTANCE;
  ADC_Get_Resistance(adc_data_owner);

  SW0_CONN;
  SW1_DISC;
  SW2_CONN;
  SW3_DISC;
  HAL_Delay(RELAY_DELAY);
  A_mf = ADC_Get_Gain();
  // `* 4` here because the output signal is attenuated to one-fourth of its original amount.
  Output_DC = ADC_Get_DC_Voltage(adc_data_owner) * 4;
  
  for (int i = 0; i < MAX_SEND_LEN; ++i)
  {
    DDS_Freq = DDS_Freq_Array[i];
    AD9833_Default_Set(DDS_Freq);
    Gain_Array[i] = ADC_Get_Gain();
    if (already && (DDS_Freq > 1000) && (Gain_Array[i] < A_mf * 0.707f))
    {
      already = false;
      freq_upper = DDS_Freq;
    }
  }
  UARTHMI_Send_Number(0, freq_upper / 1000);
  UARTHMI_Send_Number(1, (int)A_mf);
  UARTHMI_Draw_Curve_addt(0, Gain_Array, MAX_SEND_LEN, 10);
  if (!already)
  {
    // printf("up:%d, DC:%f, Ri:%f\n", freq_upper, Output_DC, Ri);
  }
  // if (Component != 0)
  // {
    Find_Fault_R();
    UARTHMI_Send_Text(5, Component);
    UARTHMI_Send_Text(6, Fault);
  // }
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  // printf("Gain:%f\n", ADC_Get_Gain());
    // HAL_Delay(1000);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
	while (1)
	{
		DAC_Output(2.5);
		HAL_Delay(350);
		DAC_Output(0.5);
		HAL_Delay(150);
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex:  */
  printf("Wrong parameters value: file %s on line %d\r\n", file, line);
  printf("%lu", ((&htim2)->Instance));
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
