/* Demo of the power of system ticks vs _delay_ms() */
/* Eight LEDs blink, each at just-slightly different frequencies */
/* Creates a fancy phasing pattern  */

// ------- Preamble -------- //
#include <avr/io.h>             
#include <util/delay.h>         
#include <avr/interrupt.h>

#include "pinDefines.h"
#include "macros.h"

// -------- Global Variables --------- //
volatile uint8_t milliseconds = 0;	

// -------- Functions --------- //
static inline void initTimerTicks(void){
  set_bit(TCCR0A, WGM01);	/* CTC mode */
  set_bit(TCCR0B, CS02);	/* 8 MHz / 256 */
  set_bit(TIMSK0, OCIE0A); 	/* output compare interrupt enable*/
  OCR0A = 30;			/* 8 Mhz / 256 / 31 = 0.992 ms */
  sei();			/* set (global) enable interrupt bit */
}

ISR(TIMER0_COMPA_vect){
  milliseconds++;
}

int main(void){
  uint8_t ledTime[8];
  uint8_t i;

  // -------- Inits --------- //
  initTimerTicks();
  LED_DDR = 0xff;		/* all output */
  set_bit(BUTTON_PORT, BUTTON);	/* enable pullup on button */

  // ------ Event loop ------ //
  while(1){	

    /* 8 LEDs flashing on different schedules */
    for (i=0 ; i<8 ; i++){
      if(milliseconds == ledTime[i]){
	toggle_bit(LED_PORT, i);
	ledTime[i] = milliseconds + 100 + i; 
      }
    }  
    
    /* Reset and resync with button press */
    if (bit_is_clear(BUTTON_IN, BUTTON)){
      for (i=0 ; i<8 ; i++){	/* resync */
	ledTime[i] = 0;
      }
      LED_PORT = 0; 		/* reset */
    }

  }    /* End event loop */
  return(0);		      /* This line is never reached  */
}

