#pragma once
#include "Common.h"

#define AD_CONT_B28         13
#define AD_CONT_RESET       8

#define AD_CONT(bits)       do{CLR(bits, 15);CLR(bits, 14);}while(false)
#define AD_CONT_RST(bits)   do{SET(bits, AD_CONT_RESET);}while(false)
#define AD_CONT_SET(bits)   do{CLR(bits, AD_CONT_RESET);}while(false)

#define AD_FREQ_1(bits)     do{CLR(bits, 15);SET(bits, 14);}while(false)
#define AD_FREQ_2(bits)     do{SET(bits, 15);CLR(bits, 14);}while(false)

#define AD_PHAS_1(bits)     do{SET(bits, 15);SET(bits, 14);CLR(bits, 13);}while(false)
#define AD_PHAS_2(bits)     do{SET(bits, 15);SET(bits, 14);SET(bits, 13);}while(false)

#define AD_VOUT_SIN(bits)   do{CLR(bits, 15);CLR(bits, 14);}while(false)

const uint32_t phase_all = 1 << 28;
const uint32_t ref_freq = 25000000; // on board 25MHz Crystal Oscillator