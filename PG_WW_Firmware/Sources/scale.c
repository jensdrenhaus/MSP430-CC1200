/*------------------------------------------------------------------------------
| File: scale.c
|
| Say something about the module. Copy that part from name.h
|
| Note: important things
 -----------------------------------------------------------------------------*/

#include "scale.h"
#include "msp430fr5969.h"



//#############################################################################
// globals


#define CONVERSION_VALUE_1_2 0.29296875 // Vref 1200mV / 4095 (12 bit ADC resolution)

static float WeightHandy=181;
static float VoltageHandy=78;

static unsigned int ZERO_OFFSET=290;
static float GRAMS_PER_MEASUREMENT;

static unsigned int ADC_value=0;

static float OldMeasure=0;
static float weight=0;



//#############################################################################
// private function prototypes
static int get_adc();
static int filter();
static float calculate_Weight(unsigned int ADC_value);


//#############################################################################
// module methods implementation:
//#############################################################################


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC module_somefunction()
//!
////////////////////////////////////////////////////////////////////////////
void scale_init(){

	GRAMS_PER_MEASUREMENT = WeightHandy/VoltageHandy;

	// GPIO Setup
	P3SEL1 |= BIT0;                           // Configure P1.1 for ADC
	P3SEL0 |= BIT0;

	while(REFCTL0 & REFGENBUSY);              // If ref generator busy, WAIT
	REFCTL0 |= REFVSEL_0 | REFON;             // Select internal ref = 1.2V


	ADC12CTL0   = ADC12SHT0_2 | ADC12ON | ADC12MSC;
	ADC12CTL1  |= ADC12SHP    | ADC12CONSEQ_1; // ADCCLK = MODOSC; sampling timer
	ADC12MCTL0 |= ADC12INCH_12 | ADC12VRSEL_1;  // A3 ADC input select; Vref=1.2V
	// CUSTOM

	ADC12CTL2 |= ADC12RES_2;                  // 12-bit conversion results

	while(!(REFCTL0 & REFGENRDY));            // Wait for reference generator
										      // to settle
}

////////////////////////////////////////////////////////////////////////////

//!  PUBLIC sen_request()
//!
////////////////////////////////////////////////////////////////////////////
float scale_request() {
	ADC12CTL0 |= ADC12ENC | ADC12SC;        // Start sampling/conversion
	ADC_value = ADC12MEM0;

	weight=calculate_Weight(ADC_value);
	return weight;
}

////////////////////////////////////////////////////////////////////////////

//!  PRIVATE converter()
//!
////////////////////////////////////////////////////////////////////////////
int get_adc() {
	return 0;
}

int filter() {
	float NewMeasure=0;
	NewMeasure=(((ADC_value*2)+2*OldMeasure)/4);
	OldMeasure=NewMeasure;
	return NewMeasure;
}

float calculate_Weight(unsigned int ADC_value){

	ADC_value=filter(ADC_value);

	ADC_value = ADC_value*CONVERSION_VALUE_1_2;
	ADC_value = ADC_value-ZERO_OFFSET;
	ADC_value = ADC_value*GRAMS_PER_MEASUREMENT;
	if (ADC_value > 60000)
		ADC_value=0;
    return ADC_value;

}










