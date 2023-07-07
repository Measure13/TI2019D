#ifndef __USART_HMI_H
#define __USART_HMI_H

#include <stdbool.h>
#include "usart.h"

extern volatile bool initialization_done, ready_to_receive, receive_done;

#define MAX_SEND_LEN 240
#define MAX_SEND_DATA 240

void UARTHMI_Forget_It(void);
void UARTHMI_Draw_Curve_addt(int index, float *pf, uint16_t num, uint8_t margin);
void UARTHMI_Send_Float(int index, float float_num);
void UARTHMI_Send_Text(uint8_t index, uint8_t wrong_info);
void UARTHMI_Send_Number(uint8_t index, int number);
void UARTHMI_ADC_Data_Display(uint16_t* adc_data_pointer);
#endif
