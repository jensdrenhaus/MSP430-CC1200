#ifndef RF_REG_CONFIG_H
#define RF_REG_CONFIG_H

#include "rf.h"


static const rfSetting_t preferredSettings[]=
{
  {RF_IOCFG2,            0x06},
  {RF_SYNC_CFG1,         0xA9},
  {RF_PREAMBLE_CFG0,     0x8A},
  {RF_IQIC,              0xC8},
  {RF_CHAN_BW,           0x10},
  {RF_MDMCFG1,           0x42},
  {RF_MDMCFG0,           0x05},
  {RF_SYMBOL_RATE2,      0x8F},
  {RF_SYMBOL_RATE1,      0x75},
  {RF_SYMBOL_RATE0,      0x10},
  {RF_AGC_REF,           0x27},
  {RF_AGC_CS_THR,        0xEE},
  {RF_AGC_CFG1,          0x11},
  {RF_AGC_CFG0,          0x94},
  {RF_FIFO_CFG,          0x00},
  {RF_FS_CFG,            0x12},
  {RF_PKT_CFG2,          0x00},
  {RF_PKT_CFG0,          0x20},
  {RF_PKT_LEN,           0xFF},
  {RF_IF_MIX_CFG,        0x1C},
  {RF_TOC_CFG,           0x03},
  {RF_MDMCFG2,           0x02},
  {RF_FREQ2,             0x56},
  {RF_FREQ1,             0xCC},
  {RF_FREQ0,             0xCC},
  {RF_IF_ADC1,           0xEE},
  {RF_IF_ADC0,           0x10},
  {RF_FS_DIG1,           0x07},
  {RF_FS_DIG0,           0xAF},
  {RF_FS_CAL1,           0x40},
  {RF_FS_CAL0,           0x0E},
  {RF_FS_CHP,            0x2B},
  {RF_FS_DIVTWO,         0x03},
  {RF_FS_DSM0,           0x33},
  {RF_FS_DVC0,           0x17},
  {RF_FS_PFD,            0x00},
  {RF_FS_PRE,            0x6E},
  {RF_FS_REG_DIV_CML,    0x1C},
  {RF_FS_SPARE,          0xAC},
  {RF_FS_VCO4,           0x11},
  {RF_FS_VCO2,           0x4B},
  {RF_FS_VCO1,           0x9C},
  {RF_FS_VCO0,           0xB5},
  {RF_IFAMP,             0x09},
  {RF_XOSC5,             0x0E},
  {RF_XOSC1,             0x03},
  {RF_LQI_VAL,           0x80},
  {RF_PARTNUMBER,        0x20},
  {RF_PARTVERSION,       0x11},
  {RF_MODEM_STATUS1,     0x10},
  {RF_MARC_STATUS1,      0x40},
  {RF_TXFIRST,           0x0A},
  {RF_TXLAST,            0x0A},
};


///*******************************************************************************
// * VARIABLES
// */
//// RX filter BW = 10.964912
//// Address config = No address check.
//// Packet length = 255
//// Symbol rate = 1.2
//// Carrier frequency = 867.999878
//// Bit rate = 1.2
//// Packet bit length = 0
//// Whitening = false
//// Manchester enable = false
//// Modulation format = 2-FSK
//// Packet length mode = Variable
//// Device address = 0
//// Deviation = 3.986359
//static const rfSetting_t preferredSettings[]= {
//    {RF_IOCFG2,             0x06}, // PKT_SYNC_RXTX
//    {RF_DEVIATION_M,        0xD1}, // Frequencey Deviation: ?
//    {RF_MODCFG_DEV_E,       0x00}, // Modulationformat, Freq Deviation: 2-FSK, ?
//    {RF_DCFILT_CFG,         0x5D}, // Digital DC Removal: komische werte
//    {RF_PREAMBLE_CFG0,      0x8A}, //
//    {RF_IQIC,               0xCB}, // Digital Image Channel Compensation
//    {RF_CHAN_BW,            0xA6}, // Channel Filter
//    {RF_MDMCFG1,            0x40}, // General Modem Parameter
//    {RF_MDMCFG0,            0x05}, // General Modem Parameter
//    {RF_SYMBOL_RATE2,       0x3F}, // Symbol Rate
//    {RF_SYMBOL_RATE1,       0x75}, // Symbol Rate
//    {RF_SYMBOL_RATE0,       0x10}, // Symbol Rate
//    {RF_AGC_REF,            0x20}, // AGC Reference Level
//    {RF_AGC_CS_THR,         0xEC}, // Carrier Sense Threshold
//    {RF_AGC_CFG1,           0x51}, // Automatic Gain Control
//    {RF_AGC_CFG0,           0xC7}, // Automatic Gain Control
//    {RF_FIFO_CFG,           0x00}, // ### FIFO (Threshold, CRC Autoflush)
//    {RF_FS_CFG,             0x12}, // Frequnecy Synthesizer (Band Select, Out of lock?)
//    {RF_PKT_CFG0,           0x20}, // ### Packet Config: variable packet length
//    {RF_PA_CFG1,            0x3F}, // Power Amplifier ?
//    {RF_PKT_LEN,            0xFF}, // ### Packet Lenght: max packet length
//    {RF_IF_MIX_CFG,         0x1C}, // IF Mix ?
//    {RF_FREQOFF_CFG,        0x22}, // Frequency Offset Correction
//    {RF_MDMCFG2,            0x0C}, // ???
//    {RF_FREQ2,              0x56}, // Frequency Configuration:
//    {RF_FREQ1,              0xCC}, // Frequency Configuration:
//    {RF_FREQ0,              0xCC}, // Frequency Configuration:
//    {RF_FS_DIG1,            0x07}, // Frequency Synthesizer Digital
//    {RF_FS_DIG0,            0xAF}, // Frequency Synthesizer Digital
//    {RF_FS_CAL1,            0x40}, // Frequency Synthesizer Calibration
//    {RF_FS_CAL0,            0x0E}, // Frequency Synthesizer Calibration
//    {RF_FS_DIVTWO,          0x03}, // Frequency Synthesizer
//    {RF_FS_DSM0,            0x33}, // Frequency Synthesizer
//    {RF_FS_DVC0,            0x17}, // Frequency Synthesizer
//    {RF_FS_PFD,             0x00}, // Frequency Synthesizer
//    {RF_FS_PRE,             0x6E}, // Frequency Synthesizer
//    {RF_FS_REG_DIV_CML,     0x14}, // Frequency Synthesizer
//    {RF_FS_SPARE,           0xAC}, // Frequency Synthesizer
//    {RF_FS_VCO0,            0xB5}, // Frequency Synthesizer
//    {RF_XOSC5,              0x0E},
//    {RF_XOSC1,              0x03},
//};

#endif
