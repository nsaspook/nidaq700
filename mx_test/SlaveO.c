

//#define DLED_DEBUG	// comment out to send real PORT data


/* Parts of this code were modified from
 *  http://www.d.umn.edu/~cprince/PubRes/Hardware/SPI/
 * examples
 *
 * Fully interrupt drived SPI slave for pic32mx remote DAQ for pic18F45K80
 * RS-232 port access 19200 bps polled
 * Port pins [5..0] D/[7..6] A are the port output pins
 * Port B pins [7..0] are the input port pins
 * 9 12bit ADC channels single-ended AN0..AN9, AN4 redirects to AN0
 * SPI has been config'd as the slave with chip select.
 *
 * Version	0.1 modify for 45k80 SSP for use with pic32mx250 on SPI
 *
 *
 * nsaspook@nsaspook.com    Nov 2013
 */

#define P45K80

#ifdef P45K80
// PIC18F45K80 Configuration Bit Settings
#include <p18f45k80.h>

// CONFIG1L
#pragma config RETEN = OFF      // VREG Sleep Enable bit (Ultra low-power regulator is Disabled (Controlled by REGSLP bit))
#pragma config INTOSCSEL = HIGH // LF-INTOSC Low-power Enable bit (LF-INTOSC in High-power mode during Sleep)
#pragma config SOSCSEL = HIGH   // SOSC Power Selection and mode Configuration bits (High Power SOSC circuit selected)
#pragma config XINST = ON      // Extended Instruction Set 

// CONFIG1H
#pragma config FOSC = INTIO2    // Oscillator (Internal RC oscillator)
#pragma config PLLCFG = ON      // PLL x4 Enable bit
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor (Disabled)
#pragma config IESO = OFF       // Internal External Oscillator Switch Over Mode (Disabled)

// CONFIG2L
#pragma config PWRTEN = OFF     // Power Up Timer (Disabled)
#pragma config BOREN = SBORDIS  // Brown Out Detect (Enabled in hardware, SBOREN disabled)
#pragma config BORV = 3         // Brown-out Reset Voltage bits (1.8V)
#pragma config BORPWR = ZPBORMV // BORMV Power level (ZPBORMV instead of BORMV is selected)

// CONFIG2H
#pragma config WDTEN = SWDTDIS        // Watchdog Timer
#pragma config WDTPS = 1024     // Watchdog Postscaler (1:8192)

// CONFIG3H
#pragma config CANMX = PORTC    // ECAN Mux bit (ECAN TX and RX pins are located on RC6 and RC7, respectively)
#pragma config MSSPMSK = MSK7   // MSSP address masking (7 Bit address masking mode)
#pragma config MCLRE = ON       // Master Clear Enable (MCLR Enabled, RE3 Disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Overflow Reset (Enabled)
#pragma config BBSIZ = BB2K     // Boot Block Size (2K word Boot Block size)

// CONFIG5L
#pragma config CP0 = OFF        // Code Protect 00800-01FFF (Disabled)
#pragma config CP1 = OFF        // Code Protect 02000-03FFF (Disabled)
#pragma config CP2 = OFF        // Code Protect 04000-05FFF (Disabled)
#pragma config CP3 = OFF        // Code Protect 06000-07FFF (Disabled)

// CONFIG5H
#pragma config CPB = OFF        // Code Protect Boot (Disabled)
#pragma config CPD = OFF        // Data EE Read Protect (Disabled)

// CONFIG6L
#pragma config WRT0 = OFF       // Table Write Protect 00800-03FFF (Disabled)
#pragma config WRT1 = OFF       // Table Write Protect 04000-07FFF (Disabled)
#pragma config WRT2 = OFF       // Table Write Protect 08000-0BFFF (Disabled)
#pragma config WRT3 = OFF       // Table Write Protect 0C000-0FFFF (Disabled)

// CONFIG6H
#pragma config WRTC = OFF       // Config. Write Protect (Disabled)
#pragma config WRTB = OFF       // Table Write Protect Boot (Disabled)
#pragma config WRTD = OFF       // Data EE Write Protect (Disabled)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protect 00800-03FFF (Disabled)
#pragma config EBTR1 = OFF      // Table Read Protect 04000-07FFF (Disabled)
#pragma config EBTR2 = OFF      // Table Read Protect 08000-0BFFF (Disabled)
#pragma config EBTR3 = OFF      // Table Read Protect 0C000-0FFFF (Disabled)

// CONFIG7H
#pragma config EBTRB = OFF      // Table Read Protect Boot (Disabled)

#endif

#include <spi.h>
#include <timers.h>
#include <adc.h>
#include <usart.h>
#include <delays.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GenericTypeDefs.h>
#include "ringbufs.h"

/* ADC master command format
 * bits 3..0 ANx input select
 *
 * CHAR and PORT command format
 * bits 3..0 data
 *
 * config status format
 * bit 7 low  for config data sent in CMD_DUMMY per uC type
 * bit 6 RS-232 receive buffer status bit, 1 if data is waiting
 * bit	5 0=ADC ref VDD, 1=ADC rec FVR=2.048
 * bit  4 0=10bit adc, 1=12bit adc
 * bits 3..0 number of ADC channels
 *
 * hardware pins
 * SPI config
 * Pin 24 SDO
 * Pin 25 SDI
 * Pin 18 SCK, input slave clock
 * Pin 7 SS	SPI select
 *
 * RS-232 config
 * EUSART2
 * Pin 29 TX2
 * Pin 30 RX2
 */

#define	TIMEROFFSET	32000           // timer0 16bit counter value for 1 second to overflow
#define SLAVE_ACTIVE	5		// Activity counter level

/* PIC Slave commands */
#define CMD_ADC_GO	0b10000000
#define CMD_ADC_GO_H	0b10010000
#define CMD_PORT_GO	0b10100000	// send data LO_NIBBLE to port buffer
#define CMD_CHAR_GO	0b10110000	// send data LO_NIBBLE to TX buffer
#define CMD_ADC_DATA	0b11000000
#define CMD_PORT_DATA	0b11010000	// send data HI_NIBBLE to port buffer ->PORT and return input PORT data in received SPI data byte
#define CMD_CHAR_DATA	0b11100000	// send data HI_NIBBLE to TX buffer and return RX buffer in received SPI data byte
#define CMD_XXXX	0b11110000	//
#define CMD_CHAR_RX	0b00010000	// Get RX buffer
#define CMD_DUMMY_CFG	0b00000000	// stuff config data in SPI buffer
#define CMD_DEAD        0b11111111      // This is usually a bad response

#ifdef P45K80
#define CMD_DUMMY	0b00111010	/* 10 channels 2.048 but only 9 are ADC,bit 6 set for rs232 data waiting */
#define NUM_AI_CHAN     10
#define UART_TX_MASK	0b10000000
#define UART_RX_MASK	0b01000000
#endif

#define	HI_NIBBLE	0xf0
#define	LO_NIBBLE	0x0f
#define	ADC_SWAP_MASK	0b01000000
#define UART_DUMMY_MASK	0b01000000

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
#define IN			HIGH
#define OUT			LOW

#define	SRQ		LATCbits.LATC2

#ifdef DLED_DEBUG
#ifdef P45K80
#define DLED0		LATDbits.LATD0
#define DLED1		LATDbits.LATD1
#define DLED2		LATDbits.LATD2
#define DLED3		LATDbits.LATD3
#define DLED4		LATDbits.LATD4
#define DLED5		LATDbits.LATD5
#define DLED6		LATAbits.LATA6
#define DLED7		LATAbits.LATA7
#endif
#else
#define DLED0		LATCbits.LATC0
#define DLED1		LATCbits.LATC0
#define DLED2		LATCbits.LATC0
#define DLED3		LATCbits.LATC0
#define DLED4		LATCbits.LATC0
#define DLED5		LATCbits.LATC0
#define DLED6		LATCbits.LATC0
#define DLED7		LATCbits.LATC0
#endif

#ifdef INTTYPES
#include <stdint.h>
#else
#define INTTYPES
/*unsigned types*/
typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;
/*signed types*/
typedef signed char int8_t;
typedef signed int int16_t;
typedef signed long int32_t;
typedef signed long long int64_t;
#endif

/*
 * cpanel testing
 */

typedef struct button_type { // Button to bit structure
	uint8_t button0 : 1;
	uint8_t button1 : 1;
	uint8_t button2 : 1;
	uint8_t button3 : 1;
	uint8_t button4 : 1;
	uint8_t button5 : 1;
	uint8_t button6 : 1;
	uint8_t button7 : 1;
	uint8_t button8 : 1;
	uint8_t button9 : 1;
	uint8_t button10 : 1;
	uint8_t button11 : 1;
	uint8_t button12 : 1;
	uint8_t button13 : 1;
	uint8_t button14 : 1;
	uint8_t button15 : 1;
} button_type;

typedef struct lamp_type { //Lamp to bit structure
	uint16_t lamp0 : 1;
	uint16_t lamp1 : 1;
	uint16_t lamp2 : 1;
	uint16_t lamp3 : 1;
	uint16_t lamp4 : 1;
	uint16_t lamp5 : 1;
	uint16_t lamp6 : 1;
	uint16_t lamp7 : 1;
	uint16_t lamp8 : 1;
	uint16_t lamp9 : 1;
	uint16_t lamp10 : 1;
	uint16_t lamp11 : 1;
	uint16_t lamp12 : 1;
	uint16_t lamp13 : 1;
	uint16_t lamp14 : 1;
	uint16_t lamp15 : 1;
} lamp_type;

typedef struct D_panel { // I/O structure
	struct button_type button;
	struct lamp_type lamp;
	uint32_t times, stimers[16];
	uint8_t checksum;
} D_panel;

union l_union { // SPI data exchange structure 
	struct lamp_type lamp;
	uint8_t l_byte[2];
};

union b_union { // SPI data exchange structure
	struct button_type button;
	uint8_t b_byte[2];
};

struct spi_link_io_type { // internal SPI link state table
	uint8_t link : 1;
	uint8_t frame : 1;
	uint8_t timeout, seq, config;
	int32_t int_count;
};

#pragma udata gpr7
volatile struct D_panel P;
volatile struct spi_link_io_type S;
#pragma udata 
void work_handler(void);
#define	PDELAY		28000	// 50hz refresh for I/O
#define SPI_CMD_RW	0b11110000
#define SPI_CMD_R_ONLY	0b11110001
#define SPI_CMD_DUMMY	0b00000000

/*
 * 
 */

struct spi_link_type { // internal state table
	uint8_t SPI_DATA : 1;
	uint8_t ADC_DATA : 1;
	uint8_t PORT_DATA : 1;
	uint8_t CHAR_DATA : 1;
	uint8_t REMOTE_LINK : 1;
	uint8_t REMOTE_DATA_DONE : 1;
	uint8_t LOW_BITS : 1;
	struct ringBufS_t *tx1, *rx1;
};

struct spi_stat_type {
	volatile uint32_t adc_count, adc_error_count, tx_int,
	port_count, port_error_count,
	char_count, char_error_count,
	slave_int_count, last_slave_int_count,
	comm_count;
	volatile uint8_t comm_ok, reconfig, reconfig_id;
};

struct serial_bounce_buffer_type {
	uint8_t data[2];
	uint32_t place;
};

volatile struct ringBufS_t ring_buf1, ring_buf2;

volatile struct spi_link_type spi_comm = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};
volatile struct spi_stat_type spi_stat = {0}, report_stat = {0};

const rom int8_t *build_date = __DATE__, *build_time = __TIME__;
volatile uint8_t data_in2, adc_buffer_ptr = 0, adc_channel = 0;
volatile uint8_t WDT_TO = FALSE, EEP_ER = FALSE;
volatile uint16_t adc_buffer[64] = {0}, adc_data_in = 0;
int8_t comm_stat_buffer[128];

void InterruptHandlerHigh(void);
//High priority interrupt vector
#pragma code InterruptVectorHigh = 0x08

void InterruptVectorHigh(void)
{
	_asm
		goto InterruptHandlerHigh //jump to interrupt routine
		_endasm
}
#pragma code

#pragma code work_interrupt = 0x18

void work_int(void)
{
	_asm goto work_handler _endasm // low
}
#pragma code

//----------------------------------------------------------------------------
// High priority interrupt routine
#pragma	tmpdata	ISRHtmpdata
#pragma interrupt InterruptHandlerHigh   nosave=section (".tmpdata")

void InterruptHandlerHigh(void)
{
	static uint8_t channel = 0, upper, command, port_tmp, char_txtmp, char_rxtmp, cmd_dummy = CMD_DUMMY, b_dummy;
	static union Timers timer;
	static union l_union l_tmp;
	static union b_union b_tmp;

	spi_stat.slave_int_count++;
	if (INTCONbits.RBIF) { // PORT B int handler
		INTCONbits.RBIF = LOW;
		b_dummy = PORTB;
	}

	if (PIE3bits.TX2IE && PIR3bits.TX2IF) {
		SRQ = HIGH;
		if (TXSTA2bits.TRMT) {
			PIE3bits.TX2IE = LOW;
			SRQ = LOW;
			spi_stat.tx_int++;
		}
	}

	if (INTCONbits.TMR0IF) { // check timer0 irq 1 second timer int handler
		INTCONbits.TMR0IF = LOW; //clear interrupt flag
		//check for TMR0 overflow
		timer.lt = TIMEROFFSET; // Copy timer value into union
		TMR0H = timer.bt[HIGH]; // Write high byte to Timer0
		TMR0L = timer.bt[LOW]; // Write low byte to Timer0
		spi_stat.comm_count++;
		if ((spi_stat.comm_count > SLAVE_ACTIVE) && spi_stat.comm_ok) {
			spi_comm.REMOTE_LINK = FALSE;
		}
		/*
		 * Timeout to reset the SPI CMD request
		 */
		if (S.timeout) {
			if (!--S.timeout) {
				S.link = FALSE;
				S.frame = FALSE;
				S.seq = 0;
			}
		}
	}

	if (PIR1bits.ADIF) { // ADC conversion complete flag
		PIR1bits.ADIF = LOW;
		spi_stat.adc_count++; // just keep count
		adc_buffer[channel] = ADRES;
		if (upper) {
			SSPBUF = (uint8_t) (adc_buffer[channel] >> 8); // stuff with upper 8 bits
		} else {
			SSPBUF = (uint8_t) adc_buffer[channel]; // stuff with lower 8 bits
		}
		spi_comm.ADC_DATA = TRUE;
		SRQ = LOW; // trigger the service request for ADC done
	}

	/* we only get this when the master  wants data, the slave never generates one
	 Master sends cmd/data  Slave sends status
	 Master sends cmd/data  Slave sends data and loads status into SPI buffer
	 */
	if (PIR1bits.SSPIF) { // SPI port SLAVE receiver
		PIR1bits.SSPIF = LOW;
		data_in2 = SSPBUF; // read the buffer quickly
		SRQ = HIGH; // reset the service request

		DLED0 = HIGH; // rx data led off
		if (PIR3bits.RC2IF) { // we need to read the buffer in sync with the *_CHAR_* commands so it's polled
			char_rxtmp = RCREG2;
			cmd_dummy |= UART_DUMMY_MASK; // We have real USART data waiting
			spi_comm.CHAR_DATA = TRUE;
		}
		command = data_in2 & HI_NIBBLE;

		S.link = TRUE;
		S.timeout = 3;

		/*
		 * We are processing the Master I/O CMD request
		 */
		if (S.frame) {
			switch (S.seq) {
			case 0:
				l_tmp.l_byte[0] = data_in2;
				SSPBUF = b_tmp.b_byte[1]; // preload the first byte into the SPI buffer
				break;
			case 1:
				l_tmp.l_byte[1] = data_in2;
				P.lamp = l_tmp.lamp;
			default:
				data_in2 = SPI_CMD_DUMMY; // make sure the data does not match the CMD code
				S.frame = FALSE;
				LATBbits.LATB2 = 0;
				break;
			}
			S.seq++;
			SRQ = LOW;
		}

		/*
		 * The master has sent a data RW command
		 */
		if ((data_in2 == SPI_CMD_RW) && !S.frame) {
			LATBbits.LATB2 = 1;
			if (spi_stat.reconfig_id == 0) {
				spi_stat.reconfig_id = 1;
				spi_stat.reconfig = TRUE;
			}
			S.frame = TRUE; // set the inprogress flag
			S.seq = 0;
			b_tmp.button = P.button;
			SSPBUF = b_tmp.b_byte[0]; // load the buffer for the next master byte
			SRQ = LOW;
		}

		if (!S.frame) {
			if (command == CMD_PORT_GO) {
				SSPBUF = PORTB; // read inputs into the buffer
				port_tmp = (data_in2 & LO_NIBBLE); // read lower 4 bits
				spi_stat.port_count++;
				spi_stat.last_slave_int_count = spi_stat.slave_int_count;
			}

			if (command == CMD_PORT_DATA) {
#ifndef	DLED_DEBUG
				PORTD = ((data_in2 & 0b00000011) << 4) | port_tmp; // PORTD pins [0..5]
				PORTA = ((data_in2 & 0b00001100) << 4); // PORTA pins [6..7]
#endif
				spi_comm.REMOTE_LINK = TRUE;
				/* reset link data timer if we are talking */
				timer.lt = TIMEROFFSET; // Copy timer value into union
				TMR0H = timer.bt[HIGH]; // Write high byte to Timer0
				TMR0L = timer.bt[LOW]; // Write low byte to Timer0
				INTCONbits.TMR0IF = LOW; //clear possible interrupt flag
				SSPBUF = cmd_dummy; // send the input data
			}

			if (command == CMD_CHAR_GO) {
				char_txtmp = (data_in2 & LO_NIBBLE); // read lower 4 bits
				DLED1 = HIGH; // rx data read
				SSPBUF = char_rxtmp; // send current receive data to master
				spi_stat.char_count++;
			}

			if (command == CMD_CHAR_DATA) { // get upper 4 bits send bits and send the data
				if (TXSTA2bits.TRMT) { // The USART send buffer is ready
					TXREG2 = ((data_in2 & LO_NIBBLE) << 4) | char_txtmp; // send data to RS-232 #2 output
					PIE3bits.TX2IE = HIGH; // enable the serial interrupt
				}
				SSPBUF = cmd_dummy; // send rx status first, the next SPI transfer will contain it.
				cmd_dummy = CMD_DUMMY; // clear rx bit
				spi_comm.CHAR_DATA = FALSE;
				spi_comm.REMOTE_LINK = TRUE;
				/* reset link data timer if we are talking */
				timer.lt = TIMEROFFSET; // Copy timer value into union
				TMR0H = timer.bt[HIGH]; // Write high byte to Timer0
				TMR0L = timer.bt[LOW]; // Write low byte to Timer0
				INTCONbits.TMR0IF = LOW; //clear possible interrupt flag
			}

			if ((command == CMD_ADC_GO) || (command == CMD_ADC_GO_H)) { // Found a ADC GO command
				if (data_in2 & ADC_SWAP_MASK) {
					upper = TRUE;
				} else {
					upper = FALSE;
				}
				channel = data_in2 & LO_NIBBLE;
#ifdef P45K80
				if (channel == 4) channel = 0; // invalid to set to 0
				if (channel > 9) channel = 0; // invalid to set to 0

				if (!ADCON0bits.GO) { // select the channel first
					ADCON0 = ((channel << 2) & 0b01111100) | (ADCON0 & 0b00000011);
					spi_comm.ADC_DATA = FALSE;
					ADCON0bits.GO = HIGH; // start a conversion
				} else {
					ADCON0bits.GO = LOW; // stop a conversion
					SSPBUF = cmd_dummy; // Tell master  we are here
					spi_comm.ADC_DATA = FALSE;
				}
#endif
			}

			if (command == CMD_ADC_DATA) {
				if (!ADCON0bits.GO) {
					if (upper) {
						SSPBUF = (uint8_t) adc_buffer[channel]; // stuff with lower 8 bits
					} else {
						SSPBUF = (uint8_t) (adc_buffer[channel] >> 8); // stuff with upper 8 bits
					}
					/* reset link data timer if we are talking */
					timer.lt = TIMEROFFSET; // Copy timer value into union
					TMR0H = timer.bt[HIGH]; // Write high byte to Timer0
					TMR0L = timer.bt[LOW]; // Write low byte to Timer0
					INTCONbits.TMR0IF = LOW; //clear possible interrupt flag
				} else {
					SSPBUF = cmd_dummy;
				}
			}
			if (command == CMD_DUMMY_CFG) {
				SSPBUF = cmd_dummy; // Tell master  we are here
				spi_stat.comm_count = 0;
				spi_stat.comm_ok = TRUE;
			}

			if (command == CMD_CHAR_RX) {
				SSPBUF = char_rxtmp; // Send current RX buffer contents
				cmd_dummy = CMD_DUMMY; // clear rx bit
			}
			/*
			 * tell the master we are ready for new data  unless waiting for a ADC conversion to complete
			 */
			if (!ADCON0bits.GO) SRQ = LOW;
		}
	}
}
#pragma	tmpdata

// Low priority interrupt routine
#pragma	tmpdata	ISRLtmpdata
#pragma interruptlow work_handler   nosave=section (".tmpdata")

/*
 *  This is the low priority ISR routine, the high ISR routine will be called during this code section
 */
void work_handler(void)
{
	static union b_union b_tmp;
	if (PIR1bits.TMR1IF) {
		P.times++;

		PIR1bits.TMR1IF = LOW; // clear TMR1 interrupt flag
		WriteTimer1(PDELAY);
		// Switches
		P.button.button0 = PORTDbits.RD0;
		P.button.button1 = PORTDbits.RD1;
		P.button.button2 = PORTDbits.RD2;
		P.button.button3 = PORTDbits.RD3;
		P.button.button4 = PORTDbits.RD4;
		P.button.button5 = PORTDbits.RD5;
		P.button.button6 = PORTDbits.RD6;
		P.button.button7 = PORTDbits.RD7;
		P.button.button8 = PORTEbits.RE0;
		P.button.button9 = PORTEbits.RE1;
		P.button.button10 = PORTEbits.RE2;
		if (!S.frame) {
			b_tmp.button = P.button;
			SSPBUF = b_tmp.b_byte[0]; // preload the first byte into the SPI buffer
		}
		// lamps
		LATAbits.LATA0 = P.lamp.lamp0;
		LATAbits.LATA1 = P.lamp.lamp1;
		LATAbits.LATA2 = P.lamp.lamp2;
		LATAbits.LATA3 = P.lamp.lamp3;
		LATBbits.LATB0 = P.lamp.lamp4;
		//	LATBbits.LATB1 = P.lamp.lamp5;
		//	LATBbits.LATB2 = P.lamp.lamp6;
		LATBbits.LATB3 = P.lamp.lamp7;

	}
}
#pragma	tmpdata

void wdtdelay(unsigned long delay, unsigned char clearit)
{
	static uint32_t dcount;
	for (dcount = 0; dcount <= delay; dcount++) { // delay a bit
		Nop();
		if (clearit) ClrWdt(); // reset the WDT timer
	};
}

void config_pic_io(void)
{
	spi_stat.reconfig_id = 1;
	if (RCONbits.TO == (uint8_t) LOW) WDT_TO = TRUE;
	if (EECON1bits.WRERR && (EECON1bits.EEPGD == (uint8_t) LOW)) EEP_ER = TRUE;
	/*
	 * default operation mode
	 */

	Close2USART();
	CloseADC();
	OSCCON = 0x70; // internal osc 16mhz, CONFIG OPTION 4XPLL for 64MHZ
	OSCTUNE = 0b01000000; // 4x pll
	SLRCON = 0x00; // all slew rates to max
	ANCON0 = 0;
	ANCON1 = 0;
	TRISA = 0x00; // all outputs
	TRISB = 0x00;
	TRISC = 0x00;
	TRISD = 0xff; // all inputs
	PADCFG1bits.RDPU = HIGH;
	TRISE = 0xff;
	PADCFG1bits.REPU = HIGH;
	LATA = 0xff;
	LATB = 0xff;
	LATC = 0xff;

	/* SPI pins setup */
	TRISAbits.TRISA5 = IN; // SS
	TRISCbits.TRISC3 = OUT; // SCK 
	TRISCbits.TRISC4 = IN; // SDI
	TRISCbits.TRISC5 = OUT; // SDO

	/* setup the SPI interface */
	OpenSPI(SLV_SSON, MODE_00, SMPMID); // Must be SMPMID in slave mode

	/* System activity timer */
	OpenTimer0(TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_256);
	WriteTimer0(TIMEROFFSET); //      start timer0 at ~1 second ticks

	/* event timer */
	OpenTimer1(T1_SOURCE_FOSC_4 & T1_16BIT_RW & T1_PS_1_8 & T1_OSC1EN_OFF & T1_SYNC_EXT_OFF, 0);
	IPR1bits.TMR1IP = 0; // set timer2 low pri interrupt
	WriteTimer1(PDELAY);

	/* clear SPI module possible flag */
	PIR1bits.SSPIF = LOW;
	S.link = FALSE;
	S.frame = FALSE;
	S.seq = 0;

	/*
	 * PORTB config
	 */
	INTCON2bits.RBPU = HIGH; // turn off weak pullups
	INTCONbits.RBIE = LOW; // disable PORTB interrupts
	IOCB = 0x00;

	/* Enable interrupt priority */
	RCONbits.IPEN = HIGH;
	/* Enable all priority interrupts */
	INTCONbits.GIEH = HIGH;
	INTCONbits.GIEL = HIGH;

	/* clear any SSP error bits */
	SSPCON1bits.WCOL = SSPCON1bits.SSPOV = LOW;
}

void config_pic(void)
{
	unsigned char dump;

	spi_stat.reconfig_id = 0;
	if (RCONbits.TO == (uint8_t) LOW) WDT_TO = TRUE;
	if (EECON1bits.WRERR && (EECON1bits.EEPGD == (uint8_t) LOW)) EEP_ER = TRUE;
	spi_comm.tx1 = &ring_buf1;
	spi_comm.rx1 = &ring_buf2;
	ringBufS_init(spi_comm.tx1);
	ringBufS_init(spi_comm.rx1);
#ifdef P45K80
	OSCCON = 0x70; // internal osc 16mhz, CONFIG OPTION 4XPLL for 64MHZ
	OSCTUNE = 0b01000000; // 4x pll
	SLRCON = 0x00; // all slew rates to max
	TRISA = 0b00111111; // [0..5] input, [6..7] outputs for LEDS
	LATA = 0b11000000;
	TRISB = 0b00111011; // RB6..7 outputs
	INTCON2bits.RBPU = 0; // turn on weak pullups
	INTCONbits.RBIE = 0; // disable PORTB interrupts
	INTCONbits.INT0IE = 0; // disable interrupt
	INTCONbits.INT0IF = 0; // disable interrupt
	INTCONbits.RBIF = LOW; // reset B flag
	IOCB = 0x00;
	TRISC = 0b10011000; // [0..2,5..6] outputs
	TRISD = 0b10000001; // [0..5] outputs and rs232 RD7 input, RD6 output
	LATD = 0xff; // all LEDS off/outputs high
	TRISE = 0b00000111; // [0..2] inputs, N/A others for 40 pin chip

	/* SPI pins setup */
	TRISCbits.TRISC3 = 1; // SCK pins clk in SLAVE
	TRISCbits.TRISC4 = 1; // SDI
	TRISCbits.TRISC5 = 0; // SDO
	TRISAbits.TRISA5 = 1; // SS2
	TRISCbits.TRISC2 = 0; // master service request output

	/* ADC channels setup */
	TRISAbits.TRISA0 = HIGH; // an0
	TRISAbits.TRISA1 = HIGH; // an1
	TRISAbits.TRISA2 = HIGH; // an2
	TRISAbits.TRISA3 = HIGH; // an3
	TRISAbits.TRISA5 = HIGH; // an4 SS don't use for analog
	TRISEbits.TRISE0 = HIGH; // an5
	TRISEbits.TRISE1 = HIGH; // an6
	TRISEbits.TRISE2 = HIGH; // an7
	TRISBbits.TRISB1 = HIGH; // an8
	TRISBbits.TRISB4 = HIGH; // an9

	/* CAN TX/RX setup, alt MUX to PORT C */
	TRISCbits.TRISC6 = 0; // digital output,CAN TX
	TRISCbits.TRISC7 = 1; // digital input, CAN RX

	/* RS-232 #2 TX/RX setup */
	TRISDbits.TRISD6 = 0; // digital output,TX
	TRISDbits.TRISD7 = 1; // digital input, RX

	OpenADC(ADC_FOSC_64 & ADC_RIGHT_JUST & ADC_2_TAD, ADC_CH0 & ADC_INT_ON, ADC_REF_VDD_VSS); // open ADC channel
	ANCON0 = 0b11101111; // analog bit enables
	ANCON1 = 0b00000011; // analog bit enables
	ADCON1 = 0b11100000; // ADC voltage ref 2.048 volts, vref- and neg channels to Vss
#endif

	PIE1bits.ADIE = HIGH; // the ADC interrupt enable bit
	IPR1bits.ADIP = HIGH; // ADC use high pri

	/* setup the SPI interface */
	OpenSPI(SLV_SSON, MODE_00, SMPMID); // Must be SMPMID in slave mode
	SSPBUF = CMD_DUMMY;

	/*
	 * Open the USART configured as
	 * 8N1, 38400 baud,  polled mode
	 */
	Open2USART(USART_TX_INT_ON & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_LOW, 51); // 64mhz osc
	SPBRGH2 = 0x00;
	SPBRG2 = 25;
	//	BAUDCON2bits.TXCKP=1;	// reverse TX
	PIE3bits.TX2IE = LOW;

	/* System activity timer, can reset the processor */
	OpenTimer0(TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_256);
	WriteTimer0(TIMEROFFSET); //      start timer0 at 1 second ticks

	/* event timer */
	OpenTimer1(T1_SOURCE_FOSC_4 & T1_16BIT_RW & T1_PS_1_8 & T1_OSC1EN_OFF & T1_SYNC_EXT_OFF, 0);
	IPR1bits.TMR1IP = 0; // set timer2 low pri interrupt
	WriteTimer1(PDELAY);

	/* clear SPI module possible flag and enable interrupts*/
	PIR1bits.SSPIF = LOW;
	PIE1bits.SSPIE = HIGH;

#ifdef P45K80
	/* Enable interrupt priority */
	RCONbits.IPEN = 1;
	dump = RCREG2; // clear receive double buffer
	dump = RCREG2;
	dump = RCREG2;
	/* Enable all high priority interrupts */
	INTCONbits.GIEH = 1;
	INTCONbits.GIEL = LOW;
#endif
	/* clear any SSP error bits */
	SSPCON1bits.WCOL = SSPCON1bits.SSPOV = LOW;

}

void check_config(void)
{
	if (spi_stat.reconfig) {
		INTCONbits.GIEH = 0;
		INTCONbits.GIEL = 0;
		spi_stat.reconfig = FALSE;
		if (spi_stat.reconfig_id == 0) {
			config_pic();
		} else {
			config_pic_io();
		}
	}
}

void main(void) /* SPI Master/Slave loopback */
{
	uint8_t stuff;

	/* configure the remote channel reset counter */
	spi_stat.comm_count = 0;
	spi_stat.comm_ok = FALSE;
	spi_comm.REMOTE_LINK = TRUE;

	spi_stat.reconfig = TRUE;
	spi_stat.reconfig_id = 0;
	check_config();

	wdtdelay(500, TRUE); // short delay after boot
	putrs2USART("\r\r\r\r\r\r\n #### \x1b[7m SPI Slave Ready! \x1b[0m ####\r\n");
	putrs2USART(" #### \x1b[7m SPI Slave Ready! \x1b[0m ####\r\n");
	LATD = 0b00111111; // all LEDS off/outputs high
	LATA = 0b11000000;
	DLED6 = DLED7 = HIGH;

	while (1) { // just loop

		check_config();

		if (SSPCON1bits.WCOL || SSPCON1bits.SSPOV) { // check for overruns/collisions
			SSPCON1bits.WCOL = SSPCON1bits.SSPOV = 0;
			spi_stat.adc_error_count = spi_stat.adc_count - spi_stat.adc_error_count;
			spi_stat.port_error_count = spi_stat.port_count - spi_stat.port_error_count;
			sprintf(comm_stat_buffer, "\r\n  error count: adc %lu, port %lu", report_stat.adc_error_count, report_stat.port_error_count);
			if (spi_stat.reconfig_id == 0)
				puts2USART(comm_stat_buffer);
		}
		if (RCSTA2bits.OERR || RCSTA2bits.FERR) {
			if (RCSTA2bits.FERR) {
				DLED1 = !DLED1;
			} else {
				DLED1 = LOW;
			}
			RCSTA2bits.CREN = LOW; // clear overrun
			RCSTA2bits.CREN = HIGH; // re-enable
			spi_stat.char_error_count = spi_stat.char_count - spi_stat.char_error_count;
			stuff = RCREG2; // read to clear frame error and dump all buffers
			stuff = RCREG2; // read to clear frame error
			stuff = RCREG2; // read to clear frame error
		}


		INTCONbits.GIEH = 0;
		report_stat = spi_stat;
		INTCONbits.GIEH = 1;
		if (spi_comm.REMOTE_LINK) {
			wdtdelay(600000, TRUE);
		} else {
			wdtdelay(600000, TRUE);
		}
		sprintf(comm_stat_buffer, "\r\n  count %lu, adc %lu, data %lu, char %lu, comm %lu, tx int %lu\r\n", report_stat.slave_int_count,
			report_stat.adc_count, report_stat.port_count, report_stat.char_count, report_stat.comm_count, report_stat.tx_int);
		if (spi_stat.reconfig_id == 0)
			puts2USART(comm_stat_buffer);
	}

}
