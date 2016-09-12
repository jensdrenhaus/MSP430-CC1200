//#include <msp430.h>
//
//#define CONVERSION_VALUE_1_2 0.29296875 // Vref 1200mV / 4095 (12 bit ADC resolution)
//
//float WeightHandy=181;
//float VoltageHandy=78;
//
//unsigned int ZERO_OFFSET=290;
//float GRAMS_PER_MEASUREMENT;
//
//unsigned int ADC_value=0;
//
//float OldMeasure=0;
//float calculate_Weight(unsigned int ADC_value);
//float weight=0;
//
//int main(void)
//{
//
//  WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT
//
//  GRAMS_PER_MEASUREMENT = WeightHandy/VoltageHandy;
//
//  // GPIO Setup
//  P1OUT &= ~BIT0;                           // Clear LED to start
//  P1DIR |= BIT0;                            // Set P1.0/LED to output
//  P1SEL1 |= BIT3;                           // Configure P1.1 for ADC
//  P1SEL0 |= BIT3;
//
//  // Disable the GPIO power-on default high-impedance mode to activate
//  // previously configured port settings
//  PM5CTL0 &= ~LOCKLPM5;
//
//
//
//  // DEFAULT
//  while(REFCTL0 & REFGENBUSY);              // If ref generator busy, WAIT
// /* REFCTL0 |= REFVSEL_0 | REFON;             // Select internal ref = 1.2V
//
//
//  ADC12CTL0	  = ADC12SHT0_2 | ADC12ON | ADC12MSC;
//  ADC12CTL1	 |= ADC12SHP 	| ADC12CONSEQ_1;    // ADCCLK = MODOSC; sampling timer
//  ADC12MCTL0 |= ADC12INCH_3 | ADC12VRSEL_1; */// A3 ADC input select; Vref=1.2V
//  // DEFAULT
//
//  // CUSTOM
//   while(REFCTL0 & REFGENBUSY);              // If ref generator busy, WAIT
//   REFCTL0 |= REFVSEL_0 | REFON;             // Select internal ref = 1.2V
//
//
//   ADC12CTL0   = ADC12SHT0_2 | ADC12ON | ADC12MSC;
//   ADC12CTL1  |= ADC12SHP    | ADC12CONSEQ_1; // ADCCLK = MODOSC; sampling timer
//   ADC12MCTL0 |= ADC12INCH_3 | ADC12VRSEL_1;  // A3 ADC input select; Vref=1.2V
//   // CUSTOM
//
//  ADC12CTL2 |= ADC12RES_2;                  // 12-bit conversion results
//  ADC12IER0 |= ADC12IE1;                    // Enable ADC conv complete interrupt
//
//
//  while(!(REFCTL0 & REFGENRDY));            // Wait for reference generator
//                                            // to settle
//
//
//  while (1)
//  {
//    __delay_cycles(3000);
//    ADC12CTL0 |= ADC12ENC | ADC12SC;        // Start sampling/conversion
//    ADC_value = ADC12MEM0;
//
//    weight=calculate_Weight(ADC_value);
//    __bis_SR_register(LPM0_bits | GIE);     // LPM0, ADC12_ISR will force exit
//    __no_operation();                       // For debugger
//
//  }
//}
//
//#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
//#pragma vector = ADC12_VECTOR
//__interrupt void ADC12_ISR(void)
//#elif defined(__GNUC__)
//void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void)
//#else
//#error Compiler not supported!
//#endif
//{
//  switch(__even_in_range(ADC12IV, ADC12IV_ADC12RDYIFG))
//  {
//    case ADC12IV_ADC12IFG0:                 // Vector 12:  ADC12MEM0 Interrupt
//    case ADC12IV_ADC12IFG1:
//    	P1OUT ^= BIT0;                      // P1.0 = 1
//    	__bic_SR_register_on_exit(LPM0_bits | GIE); // Exit active CPU and disable interrupts
//    	  break;
//    default: break;
//  }
//}
//
//
//float filterValue(float ADC_value)
//{
//	float NewMeasure=0;
//	NewMeasure=(((ADC_value*2)+2*OldMeasure)/4);
//	OldMeasure=NewMeasure;
//	return NewMeasure;
//}
//
//float calculate_Weight(unsigned int ADC_value){
//
//	ADC_value=filterValue(ADC_value);
//
//	ADC_value = ADC_value*CONVERSION_VALUE_1_2;
//	ADC_value = ADC_value-ZERO_OFFSET;
//	ADC_value = ADC_value*GRAMS_PER_MEASUREMENT;
//	if (ADC_value > 60000)
//		ADC_value=0;
//    return ADC_value;
//
//}
//
//
