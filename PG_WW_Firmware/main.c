#include "msp430fr5969.h"

// Globals
int pressed = 0;

// Defines



void main(void) {
    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer

    // Configure Timer A0 used for debouncing

    TA0CTL |= (TASSEL__SMCLK | MC__UP | ID__8);          // SMCLK/8 , UP mode
    TA0CTL |= TACLR;                 // clear to acticate new clock settings
    TA0CCR0 = 5000;                  // SMCLK/8/5000 = 25Hz => 40ms
    TA0CCTL0 &= ~CCIE;               // TACCR0 interrupt disabled

    // Configure Timer A1 used for periodic actions

    TA1CTL |= (TASSEL__SMCLK | MC__UP | ID__8);          // SMCLK/8 , UP mode
    TA1CTL |= TACLR;                 // clear to acticate new clock settings
    TA1CCR0 = 62500;                 // SMCLK/8/62500 = 2Hz => 0,5s
    TA1CCTL0 |= CCIE;                // TACCR0 interrupt enabled

    // Configure UART A0

     P2SEL1 |= BIT0 | BIT1;          // Set port function to UART
     P2SEL0 &= ~(BIT0 | BIT1);       // Set port function to UART

     UCA0CTLW0 = UCSWRST;            // Put eUSCI in reset
     UCA0CTLW0 |= UCSSEL__SMCLK;     // CLK = SMCLK
     // Baud Rate calculation
     // 1000000/(16*9600) = 6.510
     // Fractional portion = 0.510
     // User's Guide Table 21-4: UCBRSx = 0xAA
     // UCBRFx = int ( (6.510-6)*16) = 8
     UCA0BR0 = 6;                    // 8000000/16/9600
     UCA0BR1 = 0x00;                 // UCA0BR is a word register, set high byte
     UCA0MCTLW |= UCOS16 | UCBRF_8 | 0xAA00;
     UCA0CTLW0 &= ~UCSWRST;          // Initialize eUSCI
     UCA0IE |= UCRXIE;               // Enable RX Interrupt
    // Configure GPIO

    PM5CTL0 &=  ~LOCKLPM5;          // Disable the GPIO power-on default high-impedance mode

    P1OUT &= 0x00;                  // Shut down everything
    P1DIR &= 0x00;
    P4OUT &= 0x00;                  // Shut down everything
    P4DIR &= 0x00;
    // Configure GPIO LEDs
    P1DIR |= BIT0;                  // Set P1.0 to output direction
    P4DIR |= BIT6;                  // Set P4.6 to output direction
    // Configure GPIO Button
    P1DIR &= ~BIT1;                 // Set P1.1 to input direction
    P1REN |= BIT1;                  // Set P1.1 pullup/down Resistor
    P1OUT |= BIT1;                  // Select P1.1 pull-up
    P1IE  |= BIT1;                  // Enable Interrupt on P1.1
    P1IES |= BIT1;                  // High/Low Edge
    P1IFG &= ~BIT1;                 // clear P1.1 interrupt flag



    _EINT();                        // Global interrupt enable

    while(1)
    {

    }
}


#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    P1IE &= ~BIT1;                  // Disable Interrupt on P1.1
    P1IFG &= ~BIT1;                 // clear P1.1 interrupt flag
    TA0CCTL0 |= CCIE;               // TACCR0 interrupt enabled
    TA0CTL |= TACLR;                // start Timer TA0
    pressed = 1;                    // save buttonstate
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0(void)
{
    // check if button is still pressed
    if (!(P1IN & BIT1) && pressed) {
        TA0CTL |= TACLR;            // reset Timer TA0
    }
    // button released ?
    else if ((P1IN & BIT1) && pressed) {

        // ### button released action ###
        while(!(UCA0IFG&UCTXIFG));
        UCA0TXBUF = 0x0A;           // send NL
        while(!(UCA0IFG&UCTXIFG));
        UCA0TXBUF = 0x0D;           // send CR
        P1OUT ^= BIT0;              // toggle green LED
        // ###############################

        pressed = 0;
        TA0CCTL0 &= ~CCIE;          // TACCR0 interrupt disabled
        P1IFG &= ~BIT1;             // clear P1.1 interrupt flag
        P1IE |= BIT1;               // Enable Interrupt on P1.1
    }
}


#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer0_A1(void)
{
    static int num = 0x30;
    static int tick = 1;        // set state for full secound

    if (tick) {
        while(!(UCA0IFG&UCTXIFG));
        UCA0TXBUF = num;
        num++;
        if (num > '9') {
            num = 0x30;
            while(!(UCA0IFG&UCTXIFG));
            UCA0TXBUF = 0x0A;           // send NL
            while(!(UCA0IFG&UCTXIFG));
            UCA0TXBUF = 0x0D;           // send CR
        }
    }

    tick    ^=  1;      // toggle tick state
//  P4OUT   ^=  BIT6;   // toggle red LED

}

#pragma vector=USCI_A0_VECTOR
__interrupt void USCIA0RX_ISR(void)
{
    char received = UCA0RXBUF;

    while(!(UCA0IFG&UCTXIFG))
    {
        UCA0TXBUF = received;
    }

    if (received=='g')
    {
        P1OUT ^= BIT0;              // toggle green LED
    }
    else if (received == 'r')
    {
        P4OUT   ^=  BIT6;   // toggle red LED

    }

}



















//#define ONE_SECOND 800000
//
//volatile unsigned int state = 0;
//
//
//
//int button_klicked();
//
//int main(void) {
//    WDT_A_hold(WDT_A_BASE);                                   // Stop watchdog timer
//
//    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
//    GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_PIN0 );
//    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
//
//    PMM_unlockLPM5();                                         // unlock pins (fr5969 spezific pin state after reset)
//
//
//    while(1) {
//
//        if ( button_klicked()) {
//
//          if (state == 0) {
//              // turn on LED
//              GPIO_setOutputHighOnPin( GPIO_PORT_P1, GPIO_PIN0 );
//              state = 1;
//              continue;
//          }
//          if (state == 1) {
//              // turn off LED
//              GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_PIN0 );
//              state = 0;
//              continue;
//          }
//        }
//    }
//
//  return 0;
//}
//
//int button_klicked() {
//  static unsigned int pinVal;
//  pinVal = GPIO_getInputPinValue( GPIO_PORT_P1, GPIO_PIN1 );
//
//  if(pinVal == 0) {
//      _delay_cycles(1600);                                                // ca 2ms debounce
//      do {
//          pinVal = GPIO_getInputPinValue( GPIO_PORT_P1, GPIO_PIN1 );      // wait for button to be released
//      } while (pinVal == 0);
//      _delay_cycles(1600);                                                // ca 2ms debounce
//      return 1;
//  }
//
//  return 0;
//}
