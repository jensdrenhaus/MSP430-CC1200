#ifndef RF_REG_CONFIG_H
#define RF_REG_CONFIG_H

#include "rf.h"


static const rfSetting_t preferredSettings[]=
{
  {RF_IOCFG3,              0x06},
  {RF_IOCFG2,              0x06},
  {RF_IOCFG1,              0x30},
  {RF_IOCFG0,              0x3C},
  {RF_SYNC3,               0x93},
  {RF_SYNC2,               0x0B},
  {RF_SYNC1,               0x51},
  {RF_SYNC0,               0xDE},
  {RF_SYNC_CFG1,           0xA9},
  {RF_SYNC_CFG0,           0x03},
  {RF_DEVIATION_M,         0x06},
  {RF_MODCFG_DEV_E,        0x0B},
  {RF_DCFILT_CFG,          0x4C},
  {RF_PREAMBLE_CFG1,       0x14},
  {RF_PREAMBLE_CFG0,       0x8A},
  {RF_IQIC,                0xC8},
  {RF_CHAN_BW,             0x10},
  {RF_MDMCFG1,             0x42},
  {RF_MDMCFG0,             0x05},
  {RF_SYMBOL_RATE2,        0x8F},
  {RF_SYMBOL_RATE1,        0x75},
  {RF_SYMBOL_RATE0,        0x10},
  {RF_AGC_REF,             0x27},
  {RF_AGC_CS_THR,          0xEE},
  {RF_AGC_GAIN_ADJUST,     0x00},
  {RF_AGC_CFG3,            0xB1},
  {RF_AGC_CFG2,            0x20},
  {RF_AGC_CFG1,            0x11},
  {RF_AGC_CFG0,            0x94},
  {RF_FIFO_CFG,            0x00},
  {RF_DEV_ADDR,            0x00},
  {RF_SETTLING_CFG,        0x0B},
  {RF_FS_CFG,              0x12},
  {RF_WOR_CFG1,            0x08},
  {RF_WOR_CFG0,            0x21},
  {RF_WOR_EVENT0_MSB,      0x00},
  {RF_WOR_EVENT0_LSB,      0x00},
  {RF_RXDCM_TIME,          0x00},
  {RF_PKT_CFG2,            0x00},
  {RF_PKT_CFG1,            0x03},
  {RF_PKT_CFG0,            0x20},
  {RF_RFEND_CFG1,          0x0F},
  {RF_RFEND_CFG0,          0x00},
  {RF_PA_CFG1,             0x7F},
  {RF_PA_CFG0,             0x56},
  {RF_ASK_CFG,             0x0F},
  {RF_PKT_LEN,             0xFF},
  {RF_IF_MIX_CFG,          0x1C},
  {RF_FREQOFF_CFG,         0x20},
  {RF_TOC_CFG,             0x03},
  {RF_MARC_SPARE,          0x00},
  {RF_ECG_CFG,             0x00},
  {RF_MDMCFG2,             0x02},
  {RF_EXT_CTRL,            0x01},
  {RF_RCCAL_FINE,          0x00},
  {RF_RCCAL_COARSE,        0x00},
  {RF_RCCAL_OFFSET,        0x00},
  {RF_FREQOFF1,            0x00},
  {RF_FREQOFF0,            0x00},
  {RF_FREQ2,               0x56},
  {RF_FREQ1,               0xCC},
  {RF_FREQ0,               0xCC},
  {RF_IF_ADC2,             0x02},
  {RF_IF_ADC1,             0xEE},
  {RF_IF_ADC0,             0x10},
  {RF_FS_DIG1,             0x07},
  {RF_FS_DIG0,             0xAF},
  {RF_FS_CAL3,             0x00},
  {RF_FS_CAL2,             0x20},
  {RF_FS_CAL1,             0x40},
  {RF_FS_CAL0,             0x0E},
  {RF_FS_CHP,              0x28},
  {RF_FS_DIVTWO,           0x03},
  {RF_FS_DSM1,             0x00},
  {RF_FS_DSM0,             0x33},
  {RF_FS_DVC1,             0xFF},
  {RF_FS_DVC0,             0x17},
  {RF_FS_LBI,              0x00},
  {RF_FS_PFD,              0x00},
  {RF_FS_PRE,              0x6E},
  {RF_FS_REG_DIV_CML,      0x1C},
  {RF_FS_SPARE,            0xAC},
  {RF_FS_VCO4,             0x14},
  {RF_FS_VCO3,             0x00},
  {RF_FS_VCO2,             0x00},
  {RF_FS_VCO1,             0x00},
  {RF_FS_VCO0,             0xB5},
  {RF_GBIAS6,              0x00},
  {RF_GBIAS5,              0x02},
  {RF_GBIAS4,              0x00},
  {RF_GBIAS3,              0x00},
  {RF_GBIAS2,              0x10},
  {RF_GBIAS1,              0x00},
  {RF_GBIAS0,              0x00},
  {RF_IFAMP,               0x09},
  {RF_LNA,                 0x01},
  {RF_RXMIX,               0x01},
  {RF_XOSC5,               0x0E},
  {RF_XOSC4,               0xA0},
  {RF_XOSC3,               0x03},
  {RF_XOSC2,               0x04},
  {RF_XOSC1,               0x03},
  {RF_XOSC0,               0x00},
  {RF_ANALOG_SPARE,        0x00},
  {RF_PA_CFG3,             0x00},
  {RF_WOR_TIME1,           0x00},
  {RF_WOR_TIME0,           0x00},
  {RF_WOR_CAPTURE1,        0x00},
  {RF_WOR_CAPTURE0,        0x00},
  {RF_BIST,                0x00},
  {RF_DCFILTOFFSET_I1,     0x00},
  {RF_DCFILTOFFSET_I0,     0x00},
  {RF_DCFILTOFFSET_Q1,     0x00},
  {RF_DCFILTOFFSET_Q0,     0x00},
  {RF_IQIE_I1,             0x00},
  {RF_IQIE_I0,             0x00},
  {RF_IQIE_Q1,             0x00},
  {RF_IQIE_Q0,             0x00},
  {RF_RSSI1,               0x80},
  {RF_RSSI0,               0x00},
  {RF_MARCSTATE,           0x41},
  {RF_LQI_VAL,             0x00},
  {RF_PQT_SYNC_ERR,        0xFF},
  {RF_DEM_STATUS,          0x00},
  {RF_FREQOFF_EST1,        0x00},
  {RF_FREQOFF_EST0,        0x00},
  {RF_AGC_GAIN3,           0x00},
  {RF_AGC_GAIN2,           0xD1},
  {RF_AGC_GAIN1,           0x00},
  {RF_AGC_GAIN0,           0x3F},
  {RF_CFM_RX_DATA_OUT,     0x00},
  {RF_CFM_TX_DATA_IN,      0x00},
  {RF_ASK_SOFT_RX_DATA,    0x30},
  {RF_RNDGEN,              0x7F},
  {RF_MAGN2,               0x00},
  {RF_MAGN1,               0x00},
  {RF_MAGN0,               0x00},
  {RF_ANG1,                0x00},
  {RF_ANG0,                0x00},
  {RF_CHFILT_I2,           0x02},
  {RF_CHFILT_I1,           0x00},
  {RF_CHFILT_I0,           0x00},
  {RF_CHFILT_Q2,           0x00},
  {RF_CHFILT_Q1,           0x00},
  {RF_CHFILT_Q0,           0x00},
  {RF_GPIO_STATUS,         0x00},
  {RF_FSCAL_CTRL,          0x01},
  {RF_PHASE_ADJUST,        0x00},
  {RF_PARTNUMBER,          0x20},
  {RF_PARTVERSION,         0x11},
  {RF_SERIAL_STATUS,       0x00},
  {RF_MODEM_STATUS1,       0x10},
  {RF_MODEM_STATUS0,       0x00},
  {RF_MARC_STATUS1,        0x00},
  {RF_MARC_STATUS0,        0x00},
  {RF_PA_IFAMP_TEST,       0x00},
  {RF_FSRF_TEST,           0x00},
  {RF_PRE_TEST,            0x00},
  {RF_PRE_OVR,             0x00},
  {RF_ADC_TEST,            0x00},
  {RF_DVC_TEST,            0x0B},
  {RF_ATEST,               0x40},
  {RF_ATEST_LVDS,          0x00},
  {RF_ATEST_MODE,          0x00},
  {RF_XOSC_TEST1,          0x3C},
  {RF_XOSC_TEST0,          0x00},
  {RF_AES,                 0x00},
  {RF_MDM_TEST,            0x00},
  {RF_RXFIRST,             0x00},
  {RF_TXFIRST,             0x00},
  {RF_RXLAST,              0x00},
  {RF_TXLAST,              0x00},
  {RF_NUM_TXBYTES,         0x00},
  {RF_NUM_RXBYTES,         0x00},
  {RF_FIFO_NUM_TXBYTES,    0x0F},
  {RF_FIFO_NUM_RXBYTES,    0x00},
  {RF_RXFIFO_PRE_BUF,      0x00},
  {RF_AES_KEY15,           0x00},
  {RF_AES_KEY14,           0x00},
  {RF_AES_KEY13,           0x00},
  {RF_AES_KEY12,           0x00},
  {RF_AES_KEY11,           0x00},
  {RF_AES_KEY10,           0x00},
  {RF_AES_KEY9,            0x00},
  {RF_AES_KEY8,            0x00},
  {RF_AES_KEY7,            0x00},
  {RF_AES_KEY6,            0x00},
  {RF_AES_KEY5,            0x00},
  {RF_AES_KEY4,            0x00},
  {RF_AES_KEY3,            0x00},
  {RF_AES_KEY2,            0x00},
  {RF_AES_KEY1,            0x00},
  {RF_AES_KEY0,            0x00},
  {RF_AES_BUFFER15,        0x00},
  {RF_AES_BUFFER14,        0x00},
  {RF_AES_BUFFER13,        0x00},
  {RF_AES_BUFFER12,        0x00},
  {RF_AES_BUFFER11,        0x00},
  {RF_AES_BUFFER10,        0x00},
  {RF_AES_BUFFER9,         0x00},
  {RF_AES_BUFFER8,         0x00},
  {RF_AES_BUFFER7,         0x00},
  {RF_AES_BUFFER6,         0x00},
  {RF_AES_BUFFER5,         0x00},
  {RF_AES_BUFFER4,         0x00},
  {RF_AES_BUFFER3,         0x00},
  {RF_AES_BUFFER2,         0x00},
  {RF_AES_BUFFER1,         0x00},
  {RF_AES_BUFFER0,         0x00},
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