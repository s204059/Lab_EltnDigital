#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

static int PERIOD;

void main(void) {
  
  /* select CLOCKA as a PWM clock for channels 0,1,2,3 */
  PWMCLKAB_PCLKAB0 = 0;
  PWMCLK_PCLK0 = 0;
  PWMCLKAB_PCLKAB1 = 0;
  PWMCLK_PCLK1 = 0;
  PWMCLKAB_PCLKAB2 = 1;
  PWMCLK_PCLK2 = 0;
  PWMCLKAB_PCLKAB3 = 1;
  PWMCLK_PCLK3 = 0;
  
  PWMPRCLK_PCKA = 3; /* set CLOCKA prescaler to 3 */
  
  /* select the correct polarity */
  PWMPOL_PPOL0 = 1; // wave A
  PWMPOL_PPOL1 = 0; // wave B
  PWMPOL_PPOL2 = 0; // wave C
  PWMPOL_PPOL3 = 1; // wave D
  
  /* select the correct alignment */
  PWMCAE_CAE0 = 1;   /* select center alignment for wave A */
  PWMCAE_CAE1 = 1;   /* select center alignment for wave B */
  PWMCAE_CAE2 = 0;   /* select left alignment for wave C */
  PWMCAE_CAE3 = 0;   /* select left alignment for wave D */
  
  /* set the default period */
  PWMPER0 = 127;  
  PWMPER1 = 127;  
  PWMPER2 = 255;  
  PWMPER3 = 255;
  
  /* set the duty cycle */
  PWMDTY0 = 63;
  PWMDTY1 = 63;
  PWMDTY2 = 127;
  PWMDTY3 = 127; 
  
  /* reset the counter and load the channels configuration */
  PWMCNT0 = 1;
  PWMCNT1 = 1;    
  PWMCNT2 = 1;    
  PWMCNT3 = 1;
  
  /* enable the channels */
  PWME_PWME0 = 1; 
  PWME_PWME1 = 1; 
  PWME_PWME2 = 1; 
  PWME_PWME3 = 1;
  
  /* configuration of the ADC */
  ATDCTL1_SRES = 0;   /* select 8bit resolution */
  ATDCTL3_DJM = 1;    /* select right justification */
  ATDCTL2_ASCIE = 1;  /* ENABLE INTERRUPT EVERY TIME A CONVERSION IS COMPLETED */
  ATDCTL5_SCAN = 1;   /* enable continuos scanning mode */
  /* ATDCTL5_SCAN has to be configured as the last register because it enables a new conversion sequence */
      
 
	EnableInterrupts;

  for(;;) {
    _FEED_COP(); /* feeds the dog */
  } 
}

#pragma CODE_SEG NON_BANKED

interrupt 22 void TIM_int0(void)
{
    if(ATDSTAT2L_CCF0 == 1) /* control that the conversion has ended */
    {
      PERIOD = ATDDR0;
      ATDSTAT2L_CCF0 = 1;
    }
    
  /* set the PERIOD */
  PWMPER0 = PERIOD/2;  
  PWMPER1 = PERIOD/2;  
  PWMPER2 = PERIOD;  
  PWMPER3 = PERIOD;
  
  /* set the duty cycle */
  PWMDTY0 = PERIOD/4;
  PWMDTY1 = PERIOD/4;
  PWMDTY2 = PERIOD/2;
  PWMDTY3 = PERIOD/2;
}

#pragma CODE_SEG DEFAULT

