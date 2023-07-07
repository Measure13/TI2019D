#include "AD9833.h"

#define SET(bits, offset)   (bits = (bits | (1UL << offset)))
#define CLR(bits, offset)   (bits = (bits & (~(1UL << offset))))

#define ZERO(bits)          (bits = 0UL)

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

static const uint32_t phase_all = 1 << 28;
static const uint32_t ref_freq = 25000000; // on board 25MHz Crystal Oscillator
static uint16_t isc = 0x0000;//instruction

void AD9833_Init(void)
{
  SCK_1;
  CS_1;
  FSYNC_1;
}

/// @brief control MCP41010 to control the output amplitude, maximum is constrained by AD8051, about 3.20V; original Vpp is about 1.90V.
/// @param Amp ranging from 0 to 255
void AD9833_Set_Amplitude(uint8_t Amp)
{
  uint16_t temp = 0x1100;
  FSYNC_1;
  CS_0;
  temp |= Amp;
  for (uint8_t i = 0; i < 16; ++i)
  {
    SCK_0;	
	  if(temp&0x8000)
	    DAT_1;
	  else
		  DAT_0;
	  SCK_1;
		temp<<=1;
  }
  CS_1;
}

void AD9833_Transmit(uint16_t content)
{
  //SCK_1; // seems that only when SCLK is high can you reset FSYNC
  CS_1;
  FSYNC_0;
  for(int i = 0; i < 16; i++)
	{
    SCK_1;
		if (content & 0x8000)
			DAT_1;
		else
			DAT_0;
		SCK_0;
    content <<= 1;
	}
  SCK_1;
  FSYNC_1;
}

void AD9833_Reset(void)
{
    ZERO(isc);
    AD_CONT_RST(isc);
    SET(isc, AD_CONT_B28); // send 28 bits in 2 runs
    AD9833_Transmit(isc);
}

void AD9833_Set_Freq(uint32_t output_freq)
{
    ZERO(isc);
    uint32_t delta_phase = (uint32_t)((double)output_freq * phase_all / ref_freq + 0.5);
    // less than 268435455, or 0xfff ffff

    if (delta_phase > (phase_all - 1) || delta_phase < 1)
    {
        delta_phase = phase_all - 2;
    }
    isc = (uint16_t)(delta_phase & 0x00003fff); // LSB 14
    AD_FREQ_1(isc);
    AD9833_Transmit(isc);
    isc = (uint16_t)((delta_phase & 0x0fffc000) >> 14); // MSB 14
    AD_FREQ_1(isc);
    AD9833_Transmit(isc);
}

void AD9833_Set_Phase(uint16_t init_phase)
{
    AD_PHAS_1(init_phase);
    AD9833_Transmit(init_phase);
}

void AD9833_Set(void)
{
    ZERO(isc);
    AD_CONT_SET(isc);
    SET(isc, AD_CONT_B28);
    AD9833_Transmit(isc);
}

void AD9833_Default_Set(uint32_t output_freq)
{
    AD9833_Reset();
    AD9833_Set_Freq(output_freq);
    AD9833_Set_Phase(0);
    AD9833_Set();
}
