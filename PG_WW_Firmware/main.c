#include "phy.h"
#include "msp430fr5969.h"
#include <string.h>
#include "ui.h"

// Globals

const int MY_BOX_ID = 42;

// Defines

typedef struct{
	char string [20];
} Data;
Data data = {""};

typedef struct{
	double weight;
}Sensor_Data;
Sensor_Data sensor_data = {0.0};

// Prototypes
void process(char *string);
void enter();




void main(void) {

    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer

    UI_init(enter);

    //#########################################################################
    // Configure Timer A1 used for periodic actions
    //#########################################################################

    TA1CTL |= (TASSEL__SMCLK | MC__UP | ID__8);          // SMCLK/8 , UP mode
    TA1CTL |= TACLR;                 // clear to acticate new clock settings
    TA1CCR0 = 62500;                 // SMCLK/8/62500 = 2Hz => 0,5s
    TA1CCTL0 |= CCIE;                // TACCR0 interrupt enabled

    PHY_init(process);

    //#########################################################################
    // Configure UART A1 for measurement data input
    //#########################################################################

    P2SEL1 |= BIT0 | BIT1;          // Set port function to UART
    P2SEL0 &= ~(BIT0 | BIT1);		// Set port function to UART

    UCA1CTLW0 = UCSWRST;            // Put eUSCI in reset
    UCA1CTLW0 |= UCSSEL__SMCLK;     // CLK = SMCLK
    // Baud Rate calculation
    // 1000000/(16*9600) = 6.510
    // Fractional portion = 0.510
    // User's Guide Table 21-4: UCBRSx = 0xAA
    // UCBRFx = int ( (6.510-6)*16) = 8
    UCA1BR0 = 6;                    // 8000000/16/9600
    UCA1BR1 = 0x00;				    // UCA0BR is a word register, set high-byte
    UCA1MCTLW |= UCOS16 | UCBRF_8 | 0xAA00;
    UCA1CTLW0 &= ~UCSWRST;          // Initialize eUSCI
    UCA1IE |= UCRXIE;               // Enable USCI_A1 RX interrupt





    _EINT();                        // Global interrupt enable


    while(1)
    {

    }
}

void process (char* string) {
	PHY_send(string);
}

void enter(){
	UI_status_toggle();
}


//#############################################################################
// Timer A1 ISR for periodic events
//#############################################################################
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer0_A1(void)
{
	static int num = 0x30;
	static int tick = 1;        // set state for full secound

	if (tick) {

	}

	tick ^= 1;                   // toggel tick state
	UI_tick();

}

//#############################################################################
// UART A1 ISR for measurement data intup
//#############################################################################
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
  switch(__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG)) // check UART IFGs
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
      while(!(UCA1IFG&UCTXIFG));
      UCA0TXBUF = UCA1RXBUF;
      __no_operation();
      break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
  }
}

