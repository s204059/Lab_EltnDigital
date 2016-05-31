#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

#define thr1 3125 /* 1 kHz threshold */
#define thr2 1563 /* 2 kHz threshold */
#define thr3 781  /* 4 kHz threshold */

static int oldv,newv,v,T,T1,DC; /* static variables are initialized at 0 by default */

void main(void) {
  
  DDRT = 0xF0; /* configure PTT7 to PTT4 (aka the LEDs) as outputs */
  PTT = 0xF0;  /* turn off the LEDs */
  
  /* PLL is selected as clock source by default */
  
  while(!CPMUFLG_LOCK); /* wait until PLL is stable */
  
  TSCR2_PR = 1; /* set PRESCALER */
  
  TCTL4 = 0x3;     /* channel 0: capture on any edge */
  TIE_C0I = 1;     /* enable interrupt on channel 0 */
  // TSCR1_TFFCA = 1; /* enable the "fast flag clear" */
  
  TSCR1_TEN = 1; /* enable the counter */     

	EnableInterrupts;

  for(;;)
  {
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
  else
  {
    v = TC0;
    
    if(oldv > v)
      T1 = 0xFFFF - oldv + v;
     else
      T1 = v - oldv;
     
     TFLG1_C0F = 1; /* clear the flag */
  }
  
  /* LEDs MANAGEMENT */
  
  /* frequency */
  
  if(T > thr1){
    PTT_PTT5 = 1;
    PTT_PTT4 = 1;
  }
  
  if((T <= thr1)&(T > thr2)){
    PTT_PTT5 = 1;
    PTT_PTT4 = 0;
  }
  
  if((T <= thr2)&(T > thr3)){
    PTT_PTT5 = 0;
    PTT_PTT4 = 1;
  }
  
  if(T <= thr3){
    PTT_PTT5 = 0;
    PTT_PTT4 = 0;
  }
  
  /* duty cycle */
  
  DC = (T1*8)/T;
  
  if(DC < 2){
    PTT_PTT7 = 0;
    PTT_PTT6 = 0;
  }
  
  if((DC >= 2)&(DC < 4)){
    PTT_PTT7 = 0;
    PTT_PTT6 = 1;
  }
  
  if((DC >= 4)&(DC < 6)){
    PTT_PTT7 = 1;
    PTT_PTT6 = 0;
  }
  
  if(DC >= 6){
    PTT_PTT7 = 1;
    PTT_PTT6 = 1;
  }
}

#pragma CODE_SEG DEFAULT
