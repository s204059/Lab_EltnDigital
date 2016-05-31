#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

static int oldv,newv,T,DC; /* static variables are initialized at 0 by default */

#define m 4
#define q 25472
#define scale_factor 128

void main(void) {
  
  /* PLL is selected as clock source for the free running counter by default */
  
  while(!CPMUFLG_LOCK); /* wait until PLL is stable */
  
  TSCR2_PR = 1; /* set PRESCALER */
  
  TCTL4 = 0x3;     /* channel 0: capture on any edge */
  TIE_C0I = 1;     /* enable interrupt on channel 0 */
  // TSCR1_TFFCA = 1; /* enable the "fast flag clear" */
  
  TSCR1_TEN = 1; /* enable the counter */
  
  EnableInterrupts;
  
  /* PWM CONFIGURATION */
  
  /* the following instructions select CLKA as PWM clock */
  PWMCLK_PCLK0 = 0;
  PWMCLKAB_PCLKAB0 = 0; 
  
  PWMPRCLK_PCKA = 3; /* set to 3 the prescaler of CLKA */
  PWMPOL_PPOL0 = 1;  /* set polarity so that the wave will start high */
  PWMCAE_CAE0 = 0;   /* select left alignment */
  
  PWMPER0 = 255;  /* set the period as requested */
  
  PWMDTY0 = 127;  /* INITIALIZATION OF THE DUTY CYCLE */
  
  PWMCNT0 = 1;    /* reset the counter and load the channel configuration */
  
  PWME_PWME0 = 1; /*  enable the channel */
  
  for(;;) {
    _FEED_COP(); /* feeds the dog */
  }
}

#pragma CODE_SEG NON_BANKED

interrupt 8 void TIM_int0(void)
{
  
  /* PERIOD CALCULATION */
  
  if(PTIT == 1)
  {
     newv = TC0;
     
     if(oldv > newv)
      T = 0xFFFF - oldv + newv;
     else
      T = newv - oldv;
     
     oldv = newv;
     
     TFLG1_C0F = 1; /* clear the flag */
  }
  
  DC = (q - m * T)/scale_factor;
}

#pragma CODE_SEG DEFAULT

