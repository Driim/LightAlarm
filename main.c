//***************************************************************************************
//  MSP430 Light Alarm
//
//  Description: Световой будильник, управляемый с bluetooth.
//
//                MSP4302553
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P1.6|-->PWM LED
//            |                 |
//            |             P2.1|-->PWM LED
//            |                 |
//            |             P2.4|-->PWM LED
//            |                 |
//            |     P1.2/UCA0TXD|------------>
//            |                 | 115200 - 8N1
//            |     P1.1/UCA0RXD|<------------
//
//  D. Falko
//  February 2016
//  Built with Code Composer Studio v6
//***************************************************************************************
#include "lightalarm.h"

static uint16_t timer = 0;
volatile uint16_t need_command_handle = 0;

int main(void) {
	if (CALBC1_1MHZ==0xFF) {					  // If calibration constant erased
		while(1);                                 // do not load, trap CPU!!
	}

	DCOCTL = 0;                                 // Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_1MHZ;                      // Set DCO
	DCOCTL = CALDCO_1MHZ;

	WDTCTL = WDT_MDLY_8;                        // Watchdog clock source
	IE1 |= WDTIE;                               // Watchdog Interrupt enable

	// PWM config
	P1DIR |= BIT6;                               // P1.6
	P1SEL |= BIT6;                               // P1.6 TA1/2 options
	TA0CCR0 = PWM_PERIOD;                        // PWM Period
	TA0CCTL1 = OUTMOD_7;                         // CCR1 reset/set
	TA0CCR1 = PWM_PERIOD;                        // CCR1 PWM duty cycle
	TA0CTL = TASSEL_2 + MC_1;                    // SMCLK, up mode

	P2DIR |= BIT1 | BIT4;                        // P2.1 and P2.4 output
	P2SEL |= BIT1 | BIT4;                        // P2.1 and P2.4 TA1/2 options
	TA1CCR0 = PWM_PERIOD;                        // PWM Period
	TA1CCTL1 = OUTMOD_7;                         // CCR1 reset/set
	TA1CCR1 = PWM_PERIOD;                        // CCR1 PWM duty cycle
	TA1CCTL2 = OUTMOD_7;                         // CCR1 reset/set
	TA1CCR2 = PWM_PERIOD;                        // CCR1 PWM duty cycle
	TA1CTL = TASSEL_2 + MC_1;                    // SMCLK, up mode

	// UART config
	DCOCTL = 0;                               // Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
	DCOCTL = CALDCO_1MHZ;
	P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
	P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD
	UCA0CTL1 |= UCSSEL_2;                     // SMCLK
	UCA0BR0 = 104;                            // 1MHz 9600
	UCA0BR1 = 0;                              // 1MHz 9600
	UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

	while(1) {
		__bis_SR_register(LPM0_bits + GIE);      // Enter LPM0 w/interrupt

		if(need_command_handle) {
			handle_command(rec_buffer, rec_buffer_it);
		}
	}
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

	if(timer == 360) { //6 sec
		set_color(232, 55, 226);
	} else if(timer == 720) {
		set_color(0, 17, 255);
	} else if(timer == 1080) {
		set_color(0, 255, 34);
	} else if(timer == 1460) {
		set_color(208, 187, 0);
	} else if(timer == 1820) {
		timer = 0;
	}

	timer++;
}

// UART Rx ISR
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
	if(receiver_handler(UCA0RXBUF)) {
		// receiver_handler return 1 if need to handle command, so wake up
		__bic_SR_register_on_exit(LPM0_bits + GIE);
	}
}
