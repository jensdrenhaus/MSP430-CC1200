/*------------------------------------------------------------------------------
| File: rf.h
|
| Implements functionality for initialise CC1200 as well as sending and
| recieving using SPI module.
| Defines Makros for CC1200 register names
| Defines Packetlength
|
| Note: - uses PORT 3.5 int. to process feedback from CC1200 in addition to SPI
|       - implements PORT 3.5 pin ISR
|------------------------------------------------------------------------------
| Datatypes:
|     rf_setting_t    -- used to store register address - content touple
|     rf_status_t     -- used to store feedback from CC1200
|------------------------------------------------------------------------------
| Functions:
|     rf_init  -- init subjacent modules, saves functionpointer for callback
|     rf_send  -- sends data to CC1200 using SPI module
 ----------------------------------------------------------------------------*/

#ifndef _RF_H_
#define _RF_H_

#include "types.h"
#include "com.h"

/* Configuration Registers */
#define RF_IOCFG3                   0x0000
#define RF_IOCFG2                   0x0001
#define RF_IOCFG1                   0x0002
#define RF_IOCFG0                   0x0003
#define RF_SYNC3                    0x0004
#define RF_SYNC2                    0x0005
#define RF_SYNC1                    0x0006
#define RF_SYNC0                    0x0007
#define RF_SYNC_CFG1                0x0008
#define RF_SYNC_CFG0                0x0009
#define RF_DEVIATION_M              0x000A
#define RF_MODCFG_DEV_E             0x000B
#define RF_DCFILT_CFG               0x000C
#define RF_PREAMBLE_CFG1            0x000D
#define RF_PREAMBLE_CFG0            0x000E
#define RF_IQIC                     0x000F
#define RF_CHAN_BW                  0x0010
#define RF_MDMCFG1                  0x0011
#define RF_MDMCFG0                  0x0012
#define RF_SYMBOL_RATE2             0x0013
#define RF_SYMBOL_RATE1             0x0014
#define RF_SYMBOL_RATE0             0x0015
#define RF_AGC_REF                  0x0016
#define RF_AGC_CS_THR               0x0017
#define RF_AGC_GAIN_ADJUST          0x0018
#define RF_AGC_CFG3                 0x0019
#define RF_AGC_CFG2                 0x001A
#define RF_AGC_CFG1                 0x001B
#define RF_AGC_CFG0                 0x001C
#define RF_FIFO_CFG                 0x001D
#define RF_DEV_ADDR                 0x001E
#define RF_SETTLING_CFG             0x001F
#define RF_FS_CFG                   0x0020
#define RF_WOR_CFG1                 0x0021
#define RF_WOR_CFG0                 0x0022
#define RF_WOR_EVENT0_MSB           0x0023
#define RF_WOR_EVENT0_LSB           0x0024
#define RF_RXDCM_TIME               0x0025
#define RF_PKT_CFG2                 0x0026
#define RF_PKT_CFG1                 0x0027
#define RF_PKT_CFG0                 0x0028
#define RF_RFEND_CFG1               0x0029
#define RF_RFEND_CFG0               0x002A
#define RF_PA_CFG1                  0x002B
#define RF_PA_CFG0                  0x002C
#define RF_ASK_CFG                  0x002D
#define RF_PKT_LEN                  0x002E

/* Extended Configuration Registers */
#define RF_IF_MIX_CFG               0x2F00
#define RF_FREQOFF_CFG              0x2F01
#define RF_TOC_CFG                  0x2F02
#define RF_MARC_SPARE               0x2F03
#define RF_ECG_CFG                  0x2F04
#define RF_MDMCFG2                  0x2F05
#define RF_EXT_CTRL                 0x2F06
#define RF_RCCAL_FINE               0x2F07
#define RF_RCCAL_COARSE             0x2F08
#define RF_RCCAL_OFFSET             0x2F09
#define RF_FREQOFF1                 0x2F0A
#define RF_FREQOFF0                 0x2F0B
#define RF_FREQ2                    0x2F0C
#define RF_FREQ1                    0x2F0D
#define RF_FREQ0                    0x2F0E
#define RF_IF_ADC2                  0x2F0F
#define RF_IF_ADC1                  0x2F10
#define RF_IF_ADC0                  0x2F11
#define RF_FS_DIG1                  0x2F12
#define RF_FS_DIG0                  0x2F13
#define RF_FS_CAL3                  0x2F14
#define RF_FS_CAL2                  0x2F15
#define RF_FS_CAL1                  0x2F16
#define RF_FS_CAL0                  0x2F17
#define RF_FS_CHP                   0x2F18
#define RF_FS_DIVTWO                0x2F19
#define RF_FS_DSM1                  0x2F1A
#define RF_FS_DSM0                  0x2F1B
#define RF_FS_DVC1                  0x2F1C
#define RF_FS_DVC0                  0x2F1D
#define RF_FS_LBI                   0x2F1E
#define RF_FS_PFD                   0x2F1F
#define RF_FS_PRE                   0x2F20
#define RF_FS_REG_DIV_CML           0x2F21
#define RF_FS_SPARE                 0x2F22
#define RF_FS_VCO4                  0x2F23
#define RF_FS_VCO3                  0x2F24
#define RF_FS_VCO2                  0x2F25
#define RF_FS_VCO1                  0x2F26
#define RF_FS_VCO0                  0x2F27
#define RF_GBIAS6                   0x2F28
#define RF_GBIAS5                   0x2F29
#define RF_GBIAS4                   0x2F2A
#define RF_GBIAS3                   0x2F2B
#define RF_GBIAS2                   0x2F2C
#define RF_GBIAS1                   0x2F2D
#define RF_GBIAS0                   0x2F2E
#define RF_IFAMP                    0x2F2F
#define RF_LNA                      0x2F30
#define RF_RXMIX                    0x2F31
#define RF_XOSC5                    0x2F32
#define RF_XOSC4                    0x2F33
#define RF_XOSC3                    0x2F34
#define RF_XOSC2                    0x2F35
#define RF_XOSC1                    0x2F36
#define RF_XOSC0                    0x2F37
#define RF_ANALOG_SPARE             0x2F38
#define RF_PA_CFG3                  0x2F39
#define RF_IRQ0M                    0x2F3F
#define RF_IRQ0F                    0x2F40

/* Status Registers */
#define RF_WOR_TIME1                0x2F64
#define RF_WOR_TIME0                0x2F65
#define RF_WOR_CAPTURE1             0x2F66
#define RF_WOR_CAPTURE0             0x2F67
#define RF_BIST                     0x2F68
#define RF_DCFILTOFFSET_I1          0x2F69
#define RF_DCFILTOFFSET_I0          0x2F6A
#define RF_DCFILTOFFSET_Q1          0x2F6B
#define RF_DCFILTOFFSET_Q0          0x2F6C
#define RF_IQIE_I1                  0x2F6D
#define RF_IQIE_I0                  0x2F6E
#define RF_IQIE_Q1                  0x2F6F
#define RF_IQIE_Q0                  0x2F70
#define RF_RSSI1                    0x2F71
#define RF_RSSI0                    0x2F72
#define RF_MARCSTATE                0x2F73
#define RF_LQI_VAL                  0x2F74
#define RF_PQT_SYNC_ERR             0x2F75
#define RF_DEM_STATUS               0x2F76
#define RF_FREQOFF_EST1             0x2F77
#define RF_FREQOFF_EST0             0x2F78
#define RF_AGC_GAIN3                0x2F79
#define RF_AGC_GAIN2                0x2F7A
#define RF_AGC_GAIN1                0x2F7B
#define RF_AGC_GAIN0                0x2F7C
#define RF_CFM_RX_DATA_OUT          0x2F7D
#define RF_CFM_TX_DATA_IN           0x2F7E
#define RF_ASK_SOFT_RX_DATA         0x2F7F
#define RF_RNDGEN                   0x2F80
#define RF_MAGN2                    0x2F81
#define RF_MAGN1                    0x2F82
#define RF_MAGN0                    0x2F83
#define RF_ANG1                     0x2F84
#define RF_ANG0                     0x2F85
#define RF_CHFILT_I2                0x2F86
#define RF_CHFILT_I1                0x2F87
#define RF_CHFILT_I0                0x2F88
#define RF_CHFILT_Q2                0x2F89
#define RF_CHFILT_Q1                0x2F8A
#define RF_CHFILT_Q0                0x2F8B
#define RF_GPIO_STATUS              0x2F8C
#define RF_FSCAL_CTRL               0x2F8D
#define RF_PHASE_ADJUST             0x2F8E
#define RF_PARTNUMBER               0x2F8F
#define RF_PARTVERSION              0x2F90
#define RF_SERIAL_STATUS            0x2F91
#define RF_MODEM_STATUS1            0x2F92
#define RF_MODEM_STATUS0            0x2F93
#define RF_MARC_STATUS1             0x2F94
#define RF_MARC_STATUS0             0x2F95
#define RF_PA_IFAMP_TEST            0x2F96
#define RF_FSRF_TEST                0x2F97
#define RF_PRE_TEST                 0x2F98
#define RF_PRE_OVR                  0x2F99
#define RF_ADC_TEST                 0x2F9A
#define RF_DVC_TEST                 0x2F9B
#define RF_ATEST                    0x2F9C
#define RF_ATEST_LVDS               0x2F9D
#define RF_ATEST_MODE               0x2F9E
#define RF_XOSC_TEST1               0x2F9F
#define RF_XOSC_TEST0               0x2FA0
#define RF_AES                      0x2FA1
#define RF_MDM_TEST                 0x2FA2

#define RF_RXFIRST                  0x2FD2
#define RF_TXFIRST                  0x2FD3
#define RF_RXLAST                   0x2FD4
#define RF_TXLAST                   0x2FD5
#define RF_NUM_TXBYTES              0x2FD6     /* Number of bytes in TXFIFO */
#define RF_NUM_RXBYTES              0x2FD7     /* Number of bytes in RXFIFO */
#define RF_FIFO_NUM_TXBYTES         0x2FD8
#define RF_FIFO_NUM_RXBYTES         0x2FD9
#define RF_RXFIFO_PRE_BUF           0x2FDA

/* DATA FIFO Access */
#define RF_SINGLE_TXFIFO            0x003F     /*  TXFIFO  - Single accecss to Transmit FIFO */
#define RF_BURST_TXFIFO             0x007F     /*  TXFIFO  - Burst accecss to Transmit FIFO  */
#define RF_SINGLE_RXFIFO            0x00BF     /*  RXFIFO  - Single accecss to Receive FIFO  */
#define RF_BURST_RXFIFO             0x00FF     /*  RXFIFO  - Busrrst ccecss to Receive FIFO  */

/* AES Workspace */
/* AES Key */
#define RF_AES_KEY                  0x2FE0     /*  AES_KEY    - Address for AES key input  */
#define RF_AES_KEY15	        	0x2FE0
#define RF_AES_KEY14	        	0x2FE1
#define RF_AES_KEY13	        	0x2FE2
#define RF_AES_KEY12	        	0x2FE3
#define RF_AES_KEY11	        	0x2FE4
#define RF_AES_KEY10	        	0x2FE5
#define RF_AES_KEY9	                0x2FE6
#define RF_AES_KEY8	                0x2FE7
#define RF_AES_KEY7	                0x2FE8
#define RF_AES_KEY6	                0x2FE9
#define RF_AES_KEY5	                0x2FEA
#define RF_AES_KEY4	                0x2FEB
#define RF_AES_KEY3	                0x2FEC
#define RF_AES_KEY2	                0x2FED
#define RF_AES_KEY1	                0x2FEE
#define RF_AES_KEY0	                0x2FEF

/* AES Buffer */
#define RF_AES_BUFFER               0x2FF0     /*  AES_BUFFER - Address for AES Buffer     */
#define RF_AES_BUFFER15				0x2FF0
#define RF_AES_BUFFER14				0x2FF1
#define RF_AES_BUFFER13				0x2FF2
#define RF_AES_BUFFER12				0x2FF3
#define RF_AES_BUFFER11				0x2FF4
#define RF_AES_BUFFER10				0x2FF5
#define RF_AES_BUFFER9				0x2FF6
#define RF_AES_BUFFER8				0x2FF7
#define RF_AES_BUFFER7				0x2FF8
#define RF_AES_BUFFER6				0x2FF9
#define RF_AES_BUFFER5				0x2FFA
#define RF_AES_BUFFER4				0x2FFB
#define RF_AES_BUFFER3				0x2FFC
#define RF_AES_BUFFER2				0x2FFD
#define RF_AES_BUFFER1				0x2FFE
#define RF_AES_BUFFER0				0x2FFF

#define RF_LQI_CRC_OK_BM            0x80
#define RF_LQI_EST_BM               0x7F

/* Command strobe registers */
#define RF_SRES                     0x30      /*  SRES    - Reset chip. */
#define RF_SFSTXON                  0x31      /*  SFSTXON - Enable and calibrate frequency synthesizer. */
#define RF_SXOFF                    0x32      /*  SXOFF   - Turn off crystal oscillator. */
#define RF_SCAL                     0x33      /*  SCAL    - Calibrate frequency synthesizer and turn it off. */
#define RF_SRX                      0x34      /*  SRX     - Enable RX. Perform calibration if enabled. */
#define RF_STX                      0x35      /*  STX     - Enable TX. If in RX state, only enable TX if CCA passes. */
#define RF_SIDLE                    0x36      /*  SIDLE   - Exit RX / TX, turn off frequency synthesizer. */
#define RF_SAFC                     0x37      /*  AFC     - Automatic Frequency Correction */
#define RF_SWOR                     0x38      /*  SWOR    - Start automatic RX polling sequence (Wake-on-Radio) */
#define RF_SPWD                     0x39      /*  SPWD    - Enter power down mode when CSn goes high. */
#define RF_SFRX                     0x3A      /*  SFRX    - Flush the RX FIFO buffer. */
#define RF_SFTX                     0x3B      /*  SFTX    - Flush the TX FIFO buffer. */
#define RF_SWORRST                  0x3C      /*  SWORRST - Reset real time clock. */
#define RF_SNOP                     0x3D      /*  SNOP    - No operation. Returns status byte. */

/* Chip states returned in status byte */
#define RF_STATE_IDLE               0x00
#define RF_STATE_RX                 0x10
#define RF_STATE_TX                 0x20
#define RF_STATE_FSTXON             0x30
#define RF_STATE_CALIBRATE          0x40
#define RF_STATE_SETTLING           0x50
#define RF_STATE_RXFIFO_ERROR       0x60
#define RF_STATE_TXFIFO_ERROR       0x70



#define RF_PAYLOADLEN                   (COM_FRAME_LEN) // 1 < PKTLEN < 126
#define RF_BC_ADDR                      0xFF
#define RF_AP_ADDR                      0x01
#define RF_STD_ADDR                     0x02

//#############################################################################
// datatypes
//#############################################################################

typedef struct s_rf_setting_t {
  uint16  addr;
  uint8   data;
}rf_setting_t;

typedef struct s_rf_data_fix{
  uint8       length;
  com_frame_t com_frame;
  uint8       status1;
  uint8       status2;
}rf_data_fix_t;

typedef union u_rf_frame{
    uint8 array[RF_PAYLOADLEN+3];
    rf_data_fix_t frame;
}rf_frame_t;


uint8 dev_addr;
uint8 br_addr;
uint8 ap_addr;
typedef uint8 rf_status_t;




//#############################################################################
// callback function definition
//#############################################################################
//typedef void (*RF_CB)(char* buf, com_src_t src);
typedef void (*RF_CB)(uint8* buf_fix, com_src_t src);

//#############################################################################
// function prototypes
//#############################################################################

/*------------------------------------------------------------------------------
| rf_init  -- init subjacent modules, saves functionpointer for callback
|
| Parameter:
|     function ptr to callback
|
| Return:
|     -
|
 -----------------------------------------------------------------------------*/
extern void rf_init(RF_CB callback);

/*------------------------------------------------------------------------------
| rf_send  -- sends data to CC1200 using SPI module
|
| Parameter:
|     ptr to data to by sent
|
| Return:
|     -
|
 -----------------------------------------------------------------------------*/
extern void rf_send(char* data);
extern void rf_send_fix(com_frame_t* frame);



#endif // _RF_H_











































///*------------------------------------------------------------------------------
//| File: phy.h
//|
//| Implemts functionality for wired serial communication.
//|
//| Note: - uses UART A0 (backchannel UART) of MSP430 to commuinicate
//|		- implements UART AO ISR
//|-------------------------------------------------------------------------------
//| Datatypes:
//|     -
//|-------------------------------------------------------------------------------
//| Functions:
//|     PHY_init       -- init UART hardware, saves functionpointer for callback
//|     PHY_send       -- sends string via serial port
//|     USCIA0RX_ISR   -- ISR for incoming bytes
// -----------------------------------------------------------------------------*/
//
//#ifndef _PHY_H_
//#define _PHY_H_
//
//#define PHY_MAX_BUF		50
//
//
////#############################################################################
//// datatypes
//
//
//
////#############################################################################
//// callback function definition
//typedef void (*PHY_CB)(char* string);
//
////#############################################################################
//// function prototypes
//
///*------------------------------------------------------------------------------
//| PHY_init       -- init UART hardware, saves functionpointer for callback
//|
//| Parameter:
//|     function ptr to callback
//|
//| Return:
//|     -
//|
// -----------------------------------------------------------------------------*/
//extern void phy_init(PHY_CB callback);
//
///*------------------------------------------------------------------------------
//| PHY_send       -- sends string via serial port
//|
//| Parameter:
//|     pointer to string to be transmitted
//|
//| Return:
//|     -
//|
// -----------------------------------------------------------------------------*/
//extern void phy_send(char *string);
//
//
//
//#endif // _PHY_H_
