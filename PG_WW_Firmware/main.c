#include <msp430.h> 
#include <driverlib.h>

#define ONE_SECOND 800000

volatile unsigned int state = 0;



int button_klicked();

int main(void) {
    WDT_A_hold(WDT_A_BASE);	   								// Stop watchdog timer
	
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_PIN0 );
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);

    PMM_unlockLPM5();          								// unlock pins (fr5969 spezific pin state after reset)


    while(1) {

        if ( button_klicked()) {

        	if (state == 0) {
        		// turn on LED
        		GPIO_setOutputHighOnPin( GPIO_PORT_P1, GPIO_PIN0 );
        		state = 1;
        		continue;
        	}
        	if (state == 1) {
        		// turn off LED
        		GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_PIN0 );
        		state = 0;
        		continue;
        	}
        }
    }

	return 0;
}

int button_klicked() {
	static unsigned int pinVal;
	pinVal = GPIO_getInputPinValue( GPIO_PORT_P1, GPIO_PIN1 );

	if(pinVal == 0) {
		_delay_cycles(1600);  												// ca 2ms debounce
		do {
			pinVal = GPIO_getInputPinValue( GPIO_PORT_P1, GPIO_PIN1 );		// wait for button to be released
		} while (pinVal == 0);
		_delay_cycles(1600);												// ca 2ms debounce
		return 1;
	}

	return 0;
}

