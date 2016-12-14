/*------------------------------------------------------------------------------
| File: scale.h
|
| Implements functionality for reading out and processing analog single from a
| strain gauge
|
| Note: uses P3.0 as analog input
 -------------------------------------------------------------------------------
| Datatypes:
|     -
|-------------------------------------------------------------------------------
| Functions:
|     scale_init                 -- set up port configuration and ADC registers
|     scale_request              --  get a calculated weight
|     scale_get_adc              -- just trigger a sampling period get raw data
|     scale_set_zero_offset      -- calibrate zero value of conversion function
|     scale_set_calib_load_value -- calibrate ref value of conversion function
 -----------------------------------------------------------------------------*/

#include "scale.h"
#include "msp430fr5969.h"
#include "types.h"



//#############################################################################
// globals
//#############################################################################
static float CALIBRATION_LOAD_WEIGHT  =  2007.0;   // 2 Liter Wasserflasche
static float gramms_per_value;

#pragma PERSISTENT(ZERO_OFFSET)                    // save in FRAM
static float ZERO_OFFSET              =   370.0;
#pragma PERSISTENT(CALIBRATION_LOAD_VALUE)         // save in FRAM
static float CALIBRATION_LOAD_VALUE   =  2921.0;





//#############################################################################
// private function prototypes
//#############################################################################
static uint16_t calculate_weight(uint16_t ADC_val);



////////////////////////////////////////////////////////////////////////////

//!  PUBLIC scale_init()
//!
////////////////////////////////////////////////////////////////////////////
void scale_init(){

	gramms_per_value = CALIBRATION_LOAD_WEIGHT /
					   (CALIBRATION_LOAD_VALUE - ZERO_OFFSET);

	// GPIO Setup
	P3SEL1 |= BIT0;                  // Configure P3.0 for ADC
	P3SEL0 |= BIT0;

	while(REFCTL0 & REFGENBUSY);     // If ref generator busy, WAIT
	REFCTL0 |= REFVSEL_2 | REFON;    // Select internal ref = 2,5V


	ADC12CTL0 &= ~ADC12ENC;          // disable ADC to configure controll regs

	ADC12CTL0 |=  ADC12ON;           // enable core
	ADC12CTL0 |= ADC12SHT0_1;        // 8 cycles per sampling period


	ADC12CTL1 |=  ADC12SSEL_3;       // SMCLK as clock
	ADC12CTL1 |=  ADC12SHP;          // sampling timer

	// CONSEQ default                single-channel, single-conversion
	// DIV    default                1
    // PDIV   default                1
	// SHS    default                ADC12SC bit triggers sampling

	ADC12CTL2 |=  ADC12RES_2;        // 12 bit resolution

	ADC12MCTL0 |= ADC12INCH_12;      // connect A12(P3.0) to MEM register 0
	ADC12MCTL0 |= ADC12VRSEL_1;      // set V+ to VRef (2,5V), V- to AVss(0V)


	ADC12CTL0 |= ADC12ENC;           // enable ADC for conversion

}

////////////////////////////////////////////////////////////////////////////

//!  PUBLIC sen_request()
//!
////////////////////////////////////////////////////////////////////////////
uint16_t scale_request() {
	uint8_t i;
	uint32_t val = 0;
	float weight = 0;

	for(i=0; i<100; i++){
		ADC12CTL0 |=  ADC12SC;                 // Start sampling/conversion
		while(!(ADC12IFGR0 & ADC12IFG0));
		val += ADC12MEM0;
	}
	val /= (i+1);
	weight = calculate_weight(val);
	return weight;
}

////////////////////////////////////////////////////////////////////////////

//!  PUPLIC scale_get_adc
//!
////////////////////////////////////////////////////////////////////////////
uint16_t scale_get_adc() {
	uint8_t i;
	uint32_t val = 0;
	for(i=0; i<100; i++){
		ADC12CTL0 |=  ADC12SC;                 // Start sampling/conversion
		while(!(ADC12IFGR0 & ADC12IFG0));
		val += ADC12MEM0;
	}
	val /= (i+1);
	return val;
}

////////////////////////////////////////////////////////////////////////////

//!  PUBLIC scale_set_zero_offset
//!
////////////////////////////////////////////////////////////////////////////
void scale_set_zero_offset(uint16_t val){
	ZERO_OFFSET = (float)val;
}


////////////////////////////////////////////////////////////////////////////

//!  PUBLIC scale_set_calib_load_value
//!
////////////////////////////////////////////////////////////////////////////
void scale_set_calib_load_value(uint16_t val){
	CALIBRATION_LOAD_VALUE = (float)val;
	gramms_per_value = CALIBRATION_LOAD_WEIGHT /
					   (CALIBRATION_LOAD_VALUE - ZERO_OFFSET);
}

////////////////////////////////////////////////////////////////////////////

//!  PRIVATE calculate_weight
//!
////////////////////////////////////////////////////////////////////////////
uint16_t calculate_weight(uint16_t ADC_val){

	float weight = 0;
	if(ADC_val <= ZERO_OFFSET)           // avoid negative values for weight
		weight = 0;
	else
		weight = ADC_val - ZERO_OFFSET;
	weight = weight * gramms_per_value;

	return (uint16_t)weight;

}










