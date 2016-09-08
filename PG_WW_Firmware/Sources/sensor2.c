/*
 * sensor2.c
 *
 *  Created on: 19.07.2016
 *      Author: polenz
 */

#include "msp430fr5969.h"
#include <string.h>
#include <stdlib.h>

volatile unsigned int ADCvar;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT

	P1DIR &= ~BIT3;                 // Set P3.0 to input direction
	P1SEL1 |= BIT0;                           // Enable A/D channel A0
	P1SEL0 |= BIT0;

	// Configure ADC12
	ADC12CTL0 = ADC12ON | ADC12SHT0_2;        // Turn on ADC12, set sampling time
	ADC12CTL1 = ADC12SHP;                     // Use sampling timer

	ADC12CTL0 |= ADC12ENC;                    // Enable conversions

	while (1)
	{
	  ADC12CTL0 |= ADC12SC;                   // Start conversion-software trigger
	  while (!(ADC12IFGR0 & BIT0));
	  ADCvar = ADC12MEM0;                     // Read conversion result
	  __no_operation();                       // SET BREAKPOINT HERE
	}
}
/* ############################# Code Example 1 #############################
 *
 * #include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT

  // GPIO Setup
  P1OUT &= ~BIT0;                           // Clear LED to start
  P1DIR |= BIT0;                            // Set P1.0/LED to output
  P1SEL1 |= BIT3;                           // Configure P1.1 for ADC
  P1SEL0 |= BIT3;

  // Disable the GPIO power-on default high-impedance mode to activate
  // previously configured port settings
  PM5CTL0 &= ~LOCKLPM5;

  // Configure ADC12
  ADC12CTL0 = ADC12SHT0_2 | ADC12ON;        // Sampling time, S&H=16, ADC12 on
  ADC12CTL1 = ADC12SHP;                     // Use sampling timer
  ADC12CTL2 |= ADC12RES_2;                  // 12-bit conversion results
  ADC12MCTL0 |= ADC12INCH_1;                // A1 ADC input select; Vref=AVCC
  ADC12IER0 |= ADC12IE0;                    // Enable ADC conv complete interrupt

  while (1)
  {
    __delay_cycles(5000);
    ADC12CTL0 |= ADC12ENC | ADC12SC;        // Start sampling/conversion

    __bis_SR_register(LPM0_bits | GIE);     // LPM0, ADC12_ISR will force exit
    __no_operation();                       // For debugger
  }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(ADC12IV, ADC12IV_ADC12RDYIFG))
  {
    case ADC12IV_NONE:        break;        // Vector  0:  No interrupt
    case ADC12IV_ADC12OVIFG:  break;        // Vector  2:  ADC12MEMx Overflow
    case ADC12IV_ADC12TOVIFG: break;        // Vector  4:  Conversion time overflow
    case ADC12IV_ADC12HIIFG:  break;        // Vector  6:  ADC12BHI
    case ADC12IV_ADC12LOIFG:  break;        // Vector  8:  ADC12BLO
    case ADC12IV_ADC12INIFG:  break;        // Vector 10:  ADC12BIN
    case ADC12IV_ADC12IFG0:                 // Vector 12:  ADC12MEM0 Interrupt
      if (ADC12MEM0 >= 0x7ff)               // ADC12MEM0 = A1 > 0.5AVcc?
        P1OUT |= BIT0;                      // P1.0 = 1
      else
        P1OUT &= ~BIT0;                     // P1.0 = 0
      __bic_SR_register_on_exit(LPM0_bits); // Exit active CPU
      break;                                // Clear CPUOFF bit from 0(SR)
    case ADC12IV_ADC12IFG1:   break;        // Vector 14:  ADC12MEM1
    case ADC12IV_ADC12IFG2:   break;        // Vector 16:  ADC12MEM2
    case ADC12IV_ADC12IFG3:   break;        // Vector 18:  ADC12MEM3
    case ADC12IV_ADC12IFG4:   break;        // Vector 20:  ADC12MEM4
    case ADC12IV_ADC12IFG5:   break;        // Vector 22:  ADC12MEM5
    case ADC12IV_ADC12IFG6:   break;        // Vector 24:  ADC12MEM6
    case ADC12IV_ADC12IFG7:   break;        // Vector 26:  ADC12MEM7
    case ADC12IV_ADC12IFG8:   break;        // Vector 28:  ADC12MEM8
    case ADC12IV_ADC12IFG9:   break;        // Vector 30:  ADC12MEM9
    case ADC12IV_ADC12IFG10:  break;        // Vector 32:  ADC12MEM10
    case ADC12IV_ADC12IFG11:  break;        // Vector 34:  ADC12MEM11
    case ADC12IV_ADC12IFG12:  break;        // Vector 36:  ADC12MEM12
    case ADC12IV_ADC12IFG13:  break;        // Vector 38:  ADC12MEM13
    case ADC12IV_ADC12IFG14:  break;        // Vector 40:  ADC12MEM14
    case ADC12IV_ADC12IFG15:  break;        // Vector 42:  ADC12MEM15
    case ADC12IV_ADC12IFG16:  break;        // Vector 44:  ADC12MEM16
    case ADC12IV_ADC12IFG17:  break;        // Vector 46:  ADC12MEM17
    case ADC12IV_ADC12IFG18:  break;        // Vector 48:  ADC12MEM18
    case ADC12IV_ADC12IFG19:  break;        // Vector 50:  ADC12MEM19
    case ADC12IV_ADC12IFG20:  break;        // Vector 52:  ADC12MEM20
    case ADC12IV_ADC12IFG21:  break;        // Vector 54:  ADC12MEM21
    case ADC12IV_ADC12IFG22:  break;        // Vector 56:  ADC12MEM22
    case ADC12IV_ADC12IFG23:  break;        // Vector 58:  ADC12MEM23
    case ADC12IV_ADC12IFG24:  break;        // Vector 60:  ADC12MEM24
    case ADC12IV_ADC12IFG25:  break;        // Vector 62:  ADC12MEM25
    case ADC12IV_ADC12IFG26:  break;        // Vector 64:  ADC12MEM26
    case ADC12IV_ADC12IFG27:  break;        // Vector 66:  ADC12MEM27
    case ADC12IV_ADC12IFG28:  break;        // Vector 68:  ADC12MEM28
    case ADC12IV_ADC12IFG29:  break;        // Vector 70:  ADC12MEM29
    case ADC12IV_ADC12IFG30:  break;        // Vector 72:  ADC12MEM30
    case ADC12IV_ADC12IFG31:  break;        // Vector 74:  ADC12MEM31
    case ADC12IV_ADC12RDYIFG: break;        // Vector 76:  ADC12RDY
    default: break;
  }
}
 *
 *
 */

/*
 *
 * ################# Code Example 2 #########################
 *
 #include <msp430.h>

#include<stdio.h>





unsigned int ADCvar3=03;

int main(void)

{

  WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT



  CSCTL0_H = CSKEY >> 8;                    // Unlock clock registers

  CSCTL1 = DCOFSEL_3 | DCORSEL;             // Set DCO to 8MHz

  CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;

  CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers

  CSCTL0_H = 0;                             // Lock CS registers

  __no_operation();



  __bis_SR_register(GIE);     // LPM0, ADC12_ISR will force exit



   //Timer output config

  P1DIR |= BIT4;

  P1SEL0 |= BIT4;



  // Configure GPIO

  P1SEL1 |= BIT0;                           // Enable A/D channel A0, A1

  P1SEL0 |= BIT0;



  PJSEL0 |= BIT4 | BIT5; // For XT1



  // Disable the GPIO power-on default high-impedance mode to activate

  // previously configured port settings

  PM5CTL0 &= ~LOCKLPM5;





  //Set up internal reference

  while(REFCTL0 & REFGENBUSY);              // If ref generator busy, WAIT

    REFCTL0 |= REFVSEL_2 | REFON;             // Select internal ref = 2.5V

                                              // Internal Reference ON

    __delay_cycles(75);                       // Delay (~75us) for Ref to settle



  // Configure ADC12

  ADC12CTL0 |= ADC12ON | ADC12SHT0_7;        // Turn on ADC12, set sampling time(8 cycles), set multiple inputs

  ADC12CTL1 |= ADC12SHP | ADC12CONSEQ_0 | ADC12SSEL_3;  // Use sampling timer, conversion sequence to single sequence multiple channel, use sub master clock

  ADC12MCTL3 |= ADC12VRSEL_4 | ADC12INCH_3 | ADC12EOS;

  ADC12CTL2 |= ADC12RES_2;                             //resolution to 12bit

  ADC12IER0 |= ADC12IE0;                    // Enable ADC conv complete interrupt

  ADC12CTL1 |=  ADC12SHS_3;                     // Set's timerB CCR1 as trigger

  ADC12CTL0 |= ADC12ENC;                    // Enable conversions





  //Configure TimerB

  //TB0CCTL0 = CCIE;

  TB0CTL = TBSSEL__SMCLK | MC__UP | ID_1;

  //TB0EX0 = TBIDEX_2;

  TB0CCR0 = 15625;

  TB0CCR1 = 100;

  TB0CCTL1 = OUTMOD_3;


  ADC12CTL0 |= ADC12SC;

while(1)

       {

    __no_operation();                       // SET BREAKPOINT HERE

       }

}





#pragma vector = ADC12_VECTOR

__interrupt void ADC12_ISR(void)

{

  switch(__even_in_range(ADC12IV, ADC12IV_ADC12RDYIFG))

  {


    case 18:                         // Vector 18:  ADC12MEM3

       ADCvar3 = ADC12MEM3;

       while(ADC12BUSY & ADC12CTL1);

       ADC12CTL0 &= ~ADC12ENC;

       ADC12CTL0 |= ADC12ENC;

       __bic_SR_register_on_exit(LPM0_bits); // Exit active CPU

       break;

    default: break;

  }

}

 */
