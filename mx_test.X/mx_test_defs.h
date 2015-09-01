/* 
 * File:   mx_test_defs.h
 * Author: root
 *
 * Created on October 22, 2013, 1:58 PM
 */

#ifndef MX_TEST_DEFS_H
#define	MX_TEST_DEFS_H

#ifdef	__cplusplus
extern "C" {
#endif
#define	MAGIC		0x0001  				// data version checkmark

#define SDSPEED         0               // SDCARD fast SPI speed 0=fastest
#define	SDPOWER				// SD card 3.3vdc power enable
#define SPI_CS  0

#define	SDBUFFERSIZE	512l
	/* #define SDBUFFER_EXTRA  8l */
#define SDNAME_SIZE     6
#define MAXSTRLEN   512

#define RED_LED         1
#define GREEN_LED       0
#define LOW		(unsigned char)0               // digital output state levels, sink
#define	HIGH            (unsigned char)1               // digital output state levels, source
#define	ON		LOW       		//
#define OFF		HIGH			//
#define	LED_ON		HIGH       		//
#define LED_OFF		LOW			//
#define	S_ON            LOW       		// low select/on for chip/led
#define S_OFF           HIGH			// high deselect/off chip/led
#define	R_ON            LOW       		// control relay states, relay is on when output gate is low
#define R_OFF           HIGH			// control relay states
#define NO		LOW
#define YES		HIGH
#if defined(__18CXX)
#define NULL0           (unsigned char)0
#endif
#define SHIFT1          1
#define SHIFT2          2
#define SHIFT4          4
#define SHIFT6          5
#define SHIFT7          7
#define SHIFT8          8
#define SHIFT9          9
#define SHIFT16         16
#define SHIFT24         24
#define SHIFT30         30
#define	ERR1		1
#define	ERR2		2
#define	XON		0x11
#define	XOFF            0x13
#define	H		1
#define	L		2
#define	HL		3
	/* SDHC defines */
#define SDT1            1
#define SDT2            2               // SD card types
#define SDT6            6
	/* MMC/SD command (in SPI) */
#define CMD0	(0x40+0)        /* GO_IDLE_STATE */
#define CMD1	(0x40+1)        /* SEND_OP_COND (MMC) */
#define	ACMD41	(0xC0+41)       /* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)        /* SEND_IF_COND */
#define CMD9	(0x40+9)        /* SEND_CSD */
#define CMD10	(0x40+10)       /* SEND_CID */
#define CMD12	(0x40+12)       /* STOP_TRANSMISSION */
#define ACMD13	(0xC0+13)       /* SD_STATUS (SDC) */
#define CMD16	(0x40+16)       /* SET_BLOCKLEN */
#define CMD17	(0x40+17)       /* READ_SINGLE_BLOCK */
#define CMD18	(0x40+18)       /* READ_MULTIPLE_BLOCK */
#define CMD23	(0x40+23)       /* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0xC0+23)       /* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(0x40+24)       /* WRITE_BLOCK */
#define CMD25	(0x40+25)       /* WRITE_MULTIPLE_BLOCK */
#define CMD55	(0x40+55)       /* APP_CMD */
#define CMD58	(0x40+58)       /* READ_OCR */
#define CRC_ON_OFF	(0x40+59)       // turn off CRC , it's default to off anyway

	/* Control signals (Platform dependent) */

	//#define SDINFO		0		// SD card data info block
	//#define SDSTART		1		// SD card first data block
	//#define	SDC0EEP		510             // start address of SD mirror block data in EEPROM

	/* PIC Slave commands */
#define CMD_ADC_GO	0b10000000
#define CMD_ADC_GO_H	0b10010000
#define CMD_PORT_GO	0b10100000	// send data LO_NIBBLE to port buffer
#define CMD_CHAR_GO	0b10110000	// send data LO_NIBBLE to TX buffer
#define CMD_ADC_DATA	0b11000000
#define CMD_PORT_DATA	0b11010000	// send data HI_NIBBLE to port buffer ->PORT and return input PORT data in received SPI data byte
#define CMD_CHAR_DATA	0b11100000	// send data HI_NIBBLE to TX buffer and return RX buffer in received SPI data byte
#define CMD_XXXX	0b11110000	//
#define CMD_CHAR_RX	0b00010000	// Return current RX buffer and clean buffer full flag
#define CMD_DUMMY_CFG	0b00000000	// stuff config data in SPI buffer
#define UART_DUMMY_MASK	0b01000000
#define CMD_DEAD        0b11111111      // This is usually a bad response

#define P45K80  // Chip type for SPI slave
#ifdef P45K80
#define CMD_DUMMY	0b00111010	/* 10 channels 2.048 but only 9 are ADC, bit 6 set for rs232 data waiting */
#define NUM_AI_CHAN     10
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* MX_TEST_DEFS_H */

