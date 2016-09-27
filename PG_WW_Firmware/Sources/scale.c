/*------------------------------------------------------------------------------
| File: scale.c
|
| Say something about the module. Copy that part from name.h
|
| Note: important things
 -----------------------------------------------------------------------------*/

#include "scale.h"
#include "msp430fr5969.h"
// due to testing
#include "serial.h"



//#############################################################################
// globals

static float CALIBRATION_LOAD_WEIGHT  =  2009.0;  // 2 Liter Wasserflasche
static float gramms_per_value;

#pragma PERSISTENT(ZERO_OFFSET)
static float ZERO_OFFSET              =   370.0;
#pragma PERSISTENT(CALIBRATION_LOAD_VALUE)
static float CALIBRATION_LOAD_VALUE   =  2921.0;





//#############################################################################
// private function prototypes
static uint16 calculate_weight(uint16 ADC_val);


//#############################################################################
// module methods implementation:
//#############################################################################


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC module_somefunction()
//!
////////////////////////////////////////////////////////////////////////////
void scale_init(){

	gramms_per_value = CALIBRATION_LOAD_WEIGHT / (CALIBRATION_LOAD_VALUE - ZERO_OFFSET);

	// GPIO Setup
	P3SEL1 |= BIT0;                           // Configure P3.0 for ADC
	P3SEL0 |= BIT0;

	while(REFCTL0 & REFGENBUSY);              // If ref generator busy, WAIT
	REFCTL0 |= REFVSEL_2 | REFON;             // Select internal ref = 2,5V


	ADC12CTL0 &= ~ADC12ENC; // disable conversion for configuring controll regs

	ADC12CTL0 |=  ADC12ON;  // enable core
	ADC12CTL0 |= ADC12SHT0_1; // 8 cycles per sampling period


	ADC12CTL1 |=  ADC12SSEL_3;   // SMCLK as clock
	ADC12CTL1 |=  ADC12SHP;      // sampling timer
	// CONSEQ default   single-channel, single-conversion
	// DIV    default   1
    // PDIV   default   1
	// SHS    default   ADC12SC bit triggers sampling

	ADC12CTL2 |=  ADC12RES_2; // 12 bit resolution

	ADC12MCTL0 |= ADC12INCH_12; // connect A12(P3.0) to MEM register 0
	ADC12MCTL0 |= ADC12VRSEL_1;  // set V+ to VRef (2,5V), V- to AVss(0V)


	ADC12CTL0 |= ADC12ENC; // enable ADC for conversion

}

////////////////////////////////////////////////////////////////////////////

//!  PUBLIC sen_request()
//!
////////////////////////////////////////////////////////////////////////////
uint16 scale_request() {
	uint8 i;
	uint32 val = 0;
	float weight = 0;

//	ADC12CTL0 |=  ADC12SC;        // Start sampling/conversion
//	while(!(ADC12IFGR0 & ADC12IFG0));
//	val += ADC12MEM0;

	for(i=0; i<100; i++){
		ADC12CTL0 |=  ADC12SC;        // Start sampling/conversion
		while(!(ADC12IFGR0 & ADC12IFG0));
		val += ADC12MEM0;
	}
	val /= (i+1);
	weight = calculate_weight(val);
	//serial_debug_word((uint16)val);

	return weight;
}


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC
//!
////////////////////////////////////////////////////////////////////////////
void scale_set_zero_offset(uint16 val){
	ZERO_OFFSET = (float)val;
}


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC
//!
////////////////////////////////////////////////////////////////////////////
void scale_set_calib_load_value(uint16 val){
	CALIBRATION_LOAD_VALUE = (float)val;
}

////////////////////////////////////////////////////////////////////////////

//!  PRIVATE converter()
//!
////////////////////////////////////////////////////////////////////////////
uint16 scale_get_adc() {
	uint8 i;
	uint32 val = 0;
	for(i=0; i<100; i++){
		ADC12CTL0 |=  ADC12SC;        // Start sampling/conversion
		while(!(ADC12IFGR0 & ADC12IFG0));
		val += ADC12MEM0;
	}
	val /= (i+1);
	return val;
}


uint16 calculate_weight(uint16 ADC_val){

	float weight = 0;
	weight = ADC_val - ZERO_OFFSET;
	weight = weight * gramms_per_value;

	return (uint16)weight;

}










