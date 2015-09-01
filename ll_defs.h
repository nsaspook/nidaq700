/* 
 * File:   ll_defs.h
 * Author: root
 *
 * Created on July 20, 2015, 3:04 PM
 */

#ifndef LL_DEFS_H
#define	LL_DEFS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define	TIMEROFFSET	26474           // timer0 16bit counter value for 1 second to overflow
#define SLAVE_ACTIVE	10		// Activity counter level

	/* PIC Slave commands */
#define CMD_ADC_GO	0b10000000
#define CMD_ADC_GO_H	0b10010000
#define CMD_PORT_GO	0b10100000	// send data LO_NIBBLE to port buffer
#define CMD_CHAR_GO	0b10110000	// send data LO_NIBBLE to TX buffer
#define CMD_ADC_DATA	0b11000000
#define CMD_PORT_DATA	0b11010000	// send data HI_NIBBLE to port buffer ->PORT and return input PORT data in received SPI data byte
#define CMD_CHAR_DATA	0b11100000	// send data HI_NIBBLE to TX buffer and return RX buffer in received SPI data byte
#define CMD_XXXX	0b11110000	//
#define CMD_CHAR_RX	0b00010000	//
#define CMD_DUMMY_CFG	0b00000000	// stuff config data in SPI buffer
#define CMD_DEAD        0b11111111      // This is usually a bad response

#define	HI_NIBBLE	0xf0
#define	LO_NIBBLE	0x0f
#define	ADC_SWAP_MASK	0b01000000
#define UART_DUMMY_MASK	0b01000000

#define CMD_DUMMY	0b01101110	/* 14 channels 2.048 but only 13 are ADC */
#define NUM_AI_CHAN     14
#define UART_TX_MASK	0b10000000
#define UART_RX_MASK	0b01000000

	/* DIO defines */
#define LOW		(unsigned char)0        // digital output state levels, sink
#define	HIGH            (unsigned char)1        // digital output state levels, source
#define	ON		LOW       		//
#define OFF		HIGH			//
#define	S_ON            LOW       		// low select/on for chip/led
#define S_OFF           HIGH			// high deselect/off chip/led
#define	R_ON            HIGH       		// control relay states, relay is on when output gate is high, uln2803,omron relays need the CPU at 5.5vdc to drive
#define R_OFF           LOW			// control relay states
#define R_ALL_OFF       0x00
#define R_ALL_ON	0xff
#define NO		LOW
#define YES		HIGH

#define DLED0		LATCbits.LATC0
#define DLED1		LATCbits.LATC1
#define DLED2		LATCbits.LATC1
#define DLED3		LATCbits.LATC1
#define DLED4		LATCbits.LATC1
#define DLED5		LATCbits.LATC1
#define DLED6		LATCbits.LATC1
#define DLED7		LATCbits.LATC1


#ifdef	__cplusplus
}
#endif

#endif	/* LL_DEFS_H */

