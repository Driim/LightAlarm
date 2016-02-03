//***************************************************************************************
//  MSP430 Blink the LED Demo - Software Toggle P1.0
//
//  Description; Toggle P1.0 by xor'ing P1.0 inside of a software loop.
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//                MSP430x5xx
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|-->LED
//
//  J. Stevenson
//  Texas Instruments, Inc
//  July 2011
//  Built with Code Composer Studio v5
//***************************************************************************************

#include <msp430.h>				

int main(void)
{
  WDTCTL = WDT_MDLY_32;
  IE1 |= WDTIE;


  P1DIR |= BIT6;                            // P1.2 and P1.3 output
  P1SEL |= BIT6;                            // P1.2 and P1.3 TA1/2 options
  TA0CCR0 = 512-1;                             // PWM Period
  TA0CCTL1 = OUTMOD_7;                         // CCR1 reset/set
  TA0CCR1 = 400;                               // CCR1 PWM duty cycle
  TA0CTL = TASSEL_2 + MC_1;                  // SMCLK, up mode

  P2DIR |= BIT1 | BIT4;                            // P1.2 and P1.3 output
  P2SEL |= BIT1 | BIT4;                            // P1.2 and P1.3 TA1/2 options
  TA1CCR0 = 512-1;                             // PWM Period
  TA1CCTL1 = OUTMOD_7;                         // CCR1 reset/set
  TA1CCR1 = 400;                               // CCR1 PWM duty cycle
  TA1CCTL2 = OUTMOD_7;                         // CCR1 reset/set
  TA1CCR2 = 400;                               // CCR1 PWM duty cycle
  TA1CTL = TASSEL_2 + MC_1;                  // SMCLK, up mode

  __bis_SR_register(CPUOFF + GIE);          // Enter LPM0 w/interrupt
}

// Watchdog Timer interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void)
#else
#error Compiler not supported!
#endif
{
  if(TA0CCR1 <= 100) {
	  TA0CCR1 = 500;
  } else {
	  TA0CCR1 -= 20;
  }

  if(TA1CCR1 >= 500) {
  	  TA1CCR1 = 100;
    } else {
  	  TA1CCR1 += 20;
    }

  if(TA1CCR2 <= 100) {
  	  TA1CCR2 = 500;
    } else {
  	  TA1CCR2 -= 20;
    }
}
