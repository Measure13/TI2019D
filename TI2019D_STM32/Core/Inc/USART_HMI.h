#ifndef __USART_HMI_H
#define __USART_HMI_H

#include <stdbool.h>
#include "usart.h"

extern bool initialization_done, ready_to_receive, receive_done;
void UARTHMI_Draw_Curve_addt(float* pf, uint16_t num);
void UARTHMI_Send_Float(int index, float float_num);
void UARTHMI_Send_Text(uint8_t index, uint8_t wrong_info);
void UARTHMI_Send_Number(uint8_t index, int number);
#endif
