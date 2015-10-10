/* Test code for SPI#1 SDCARD and PIC18f45K80 DAQ SPI#2 slave
 * It just setups and configures the ports so test routines can exercise the code and hardware for testing.
 * The RS-232 port access functions are very useful for debugging low pin count pic32mx devices
 * Version	0.1	Started adding ChaN FatFS routines for MSDOS SDCARD usage
 *		0.2	Start to swap the SPI ports so the buss can be 5v tolerant using PPS ports
 *
 * External Slave connections
 * Slave #1 select pin: B3
 * Slave #2 select pin: A3
 *
 * Slave1 Inputs struct INBITS
 * SD card eject button is bit 0 'eject'
 * SD card detect switch is bit 5 'card_detect'
 *
 * Slave1 outputs struct OUTBITS
 * SD card eject lamp is bit 2 'eject_led'
 */

// PIC32MX250F128B Configuration Bit Settings

#include <p32xxxx.h>

// DEVCFG3
// USERID = No Setting
#pragma config PMDL1WAY = ON            // Peripheral Module Disable Configuration (Allow only one reconfiguration)
#pragma config IOL1WAY = ON             // Peripheral Pin Select Configuration (Allow only one reconfiguration)
#pragma config FUSBIDIO = ON            // USB USID Selection (Controlled by the USB Module)
#pragma config FVBUSONIO = ON           // USB VBUS ON Selection (Controlled by USB Module)

// DEVCFG2
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multipliere (20x Multiplier)
#pragma config UPLLIDIV = DIV_12        // USB PLL Input Divider (12x Divider)
#pragma config UPLLEN = OFF             // USB PLL Enable (Disabled and Bypassed)
#pragma config FPLLODIV = DIV_2         // System PLL Output Clock Divider (PLL Divide by 2, 48mhz)

// DEVCFG1
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Fast RC Osc w/Div-by-N (FRCDIV))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Enabled)
#pragma config IESO = ON                // Internal/External Switch Over (Enabled)
#pragma config POSCMOD = EC             // Primary Oscillator Configuration 
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin
#pragma config FPBDIV = DIV_4           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/8)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable (Watchdog Timer is in Non-Window Mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Enabled)
//#pragma config FWDTWINSZ = WISZ_25      // Watchdog Timer Window Size (Window Size is 25%)

// DEVCFG0
#pragma config JTAGEN = OFF              // JTAG 
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

#include "mx_test.h"

#define CONFIG          (CN_ON | CN_IDLE_CON)
#define PINS            (CN15_ENABLE)
#define PULLUPS         (CN15_PULLUP_ENABLE)
#define INTERRUPT       (CHANGE_INT_ON | CHANGE_INT_PRI_2)
#define SYS_FREQ         (50000000L)
// Definitions
#define	MIN_SPI_TXFER_SIZE 8 // min number of words per transfer
#define	MAX_SPI_TXFER_SIZE 512 // max number of words per transfer


/* 
 * File:   test_main.c
 * Author: root
 *
 * Created on September 22, 2013, 5:52 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mx_test_defs.h"
#include "mx_test_types.h"
#include "sdspi.h"
#include "src/ff.h"
#include "blinker.h"

#define	ADSCALE		4095	// for unsigned conversion 12 sig bits
#define	ADREF		2.048	// internal voltage ref on 45K80
#define	ADOFFSET	0.0015	// correct for zero shift
#define ADGAIN		0.983	// correct for voltage offset from calibration value 2.000 volts

/*
 * SDCARD variables and buffer storage
 */
VOLUME_INFO_TYPE MMC_volume_Info = {0}, *vinf = 0;
uint8_t csd[18] = {0}, cid[18] = {0}, ocr[4] = {0};
volatile SDCARD_TYPE SDC0 = {MAGIC, 0, FALSE, FALSE}; // active program SD buffer
int cmd_response = 0, cmd_response_char = 0, cmd_response_port = 0, SD_NOTRDY = STA_NOINIT;
int cmd_data[3] = {0};
volatile struct V_data V;
volatile int32_t spi_flag;

FATFS FatFs; /* File system object */
FIL File[2]; /* File objects */



//*****************************************************************************
// DelayMs creates a delay of some milliseconds using the Core Timer
//

void DelayMs(WORD delay)
{
	unsigned int int_status;
	while (delay--) {
		int_status = INTDisableInterrupts();
		OpenCoreTimer(500000 / 2000);
		INTRestoreInterrupts(int_status);
		mCTClearIntFlag();
		while (!mCTGetIntFlag());
	}
	mCTClearIntFlag();
}

void Show_MMC_Info(void)
{
	char comm_buffer[128];
	if (SDC0.sddetect) {
		sprintf(comm_buffer, "\r\n Mounted %s, Serial Number %u, Size %u MB\r\n", vinf->name, (unsigned int) vinf->serial, (unsigned int) vinf->size_MB);
		SpiStringWrite(comm_buffer);
	}

}

/*
 * SPI service request interrupt
 */
void __ISR(_EXTERNAL_1_VECTOR, IPL2AUTO) External_Interrupt_1(void)
{
	spi_flag = HIGH;
	mINT1ClearIntFlag();
}

int main(void)
{
	int a = 0;
	unsigned char data_mix = 0, data_mix_old = 0;
	SDEV_TYPE1 S1 = {0}, *S1_p = &S1;
	char comm_buffer[MAXSTRLEN];
	int update_rate = 4;
	int eresult = 0, records = 0, file_errors = 0;
	double Vval, Vcal0;

	// Init remote device data
	S1.obits.out_byte = 0xff;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//STEP 1. Configure cache, wait states and peripheral bus clock
	// Configure the device for maximum performance but do not change the PBDIV
	// Given the options, this function will change the flash wait states, RAM
	// wait state and enable prefetch cache but will not change the PBDIV.
	// The PBDIV value is already set via the pragma FPBDIV option above..
	SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
	srand(ReadCoreTimer()); // Seed the pseudo random generator

	// SCLK1 pin 25 - SD pin 5
	PORTSetPinsDigitalOut(IOPORT_B, BIT_14); // Clock output
	// SCLK2 pin 26 - PIC pin 18
	PORTSetPinsDigitalOut(IOPORT_B, BIT_15); // Clock output

	PPSInput(2, SDI1, RPB8); //Assign SDI1 to pin 17 - SD pin 7
	PORTSetPinsDigitalIn(IOPORT_B, BIT_8); // Input for SDI1
	PPSInput(3, SDI2, RPB13); //Assign SDI2 to pin 24
	PORTSetPinsDigitalIn(IOPORT_B, BIT_13); // Input for SDI2
	PPSOutput(2, RPB11, SDO1); // Set 22 pin as output for SDO1 - SD pin 2
	PPSOutput(2, RPB5, SDO2); // Set 14 pin as output for SDO2
	PORTSetPinsDigitalOut(IOPORT_B, BIT_2); // CS line pin 6 - SD pin 1
	mPORTBSetBits(BIT_2); // deselect SDCARD
	PORTSetPinsDigitalOut(IOPORT_B, BIT_3); // select pin enable for SPI slaves bit 2 to 74hc138
	mPORTBSetBits(BIT_3); // deselect Slave1
	PORTSetPinsDigitalIn(IOPORT_A, BIT_3); // for SPI 2 slave interrupt input
	PPSInput(4, INT1, RPA3); // EXT Interrupt #1 Port A3 chip pin 10
	ConfigINT1(EXT_INT_PRI_2 | FALLING_EDGE_INT | EXT_INT_ENABLE);
	SetSubPriorityINT1(EXT_INT_SUB_PRI_3);
	mINT1ClearIntFlag(); // clear before enable
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Diag Led pins
	PORTSetPinsDigitalOut(IOPORT_A, BIT_0 | BIT_1); // bi-color LED
	PORTSetPinsDigitalOut(IOPORT_B, BIT_0 | BIT_1); // programming inputs /device select outputs address for SPI slaves bits [0..1] 74hc138

	// Enable multi-vectored interrupts
	INTEnableSystemMultiVectoredInt();

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// STEP 3. initialize the port pin states = outputs low
	mPORTASetBits(BIT_0 | BIT_1);
	mPORTBClearBits(BIT_0 | BIT_1);
	RTC_Init(); // Start Timer5 for background processes and 1ms soft timers
	Blink_Init(); // Start Timer4 background blink driver
	blink_led(RED_LED, LED_ON, FALSE);

	init_spi_ports();
	ps_select(0); // select the pic18 slave1 on spi port 2, select 0
	// send some text to clean the buffers
	SpiStringWrite("\r\n                                                                              \n\r");

	eresult = f_mount(&FatFs, "0:", 1);
	if (eresult) {
		sprintf(comm_buffer, "\r\n Mount error %i ", eresult);
		SpiStringWrite(comm_buffer); /* Register work area to the logical drive 1 */
		blink_led(GREEN_LED, LED_OFF, FALSE);
		blink_led(RED_LED, LED_ON, FALSE);
	} else {
		Show_MMC_Info();
		blink_led(RED_LED, LED_OFF, FALSE);
		blink_led(GREEN_LED, LED_ON, TRUE);
	}

	while (1) {
		eresult = SpiIOPoll(0x12);
		sprintf(comm_buffer, " IO Poll %i \r\n", eresult);
		if (eresult & 0x01)
			SpiStringWrite(comm_buffer);
//		V.Timer1 = update_rate;
//		while (V.Timer1);
	}

	/* Create destination file on the drive 1 */
	eresult = f_open(&File[0], "0:logfile.txt", FA_CREATE_ALWAYS | FA_WRITE);

	while (1) { // loop and move data
		V.Timer1 = update_rate;
		while (V.Timer1);

		/* loop back testing */
		data_mix_old = data_mix; // save the old data
		data_mix = rand(); // make new data
		if (S1_p->char_ready || !S1_p->ibits.in_bits.eject) {
			S1_p->rec_data = S1_p->rec_tmp;

			//			led1_green();
		} else {

			//			led1_off();
		}

		S1_p->ibits.in_byte = SpiPortWrite(S1_p->obits.out_byte);
		if (S1_p->ibits.in_bits.card_detect) {
			S1_p->obits.out_bits.eject_led = OFF;
			SD_NOTRDY = STA_NOINIT;
		} else {
			S1_p->obits.out_bits.eject_led = ~S1_p->obits.out_bits.eject_led;
		}
		for (S1_p->chan_no = 0; S1_p->chan_no < 4; S1_p->chan_no++) {
			S1_p->adc_data[S1_p->chan_no] = SpiADCRead(S1_p->chan_no);
		}
		a = 0;

		if (((records % 100)) == 0 && !eresult) {
			Vval = S1_p->adc_data[0];
			Vcal0 = ((Vval / ADSCALE * ADREF) + ADOFFSET) * ADGAIN;
			if (S1_p->char_ready) {
				snprintf(comm_buffer, 64, "\r\n  R data %x , %i, %x , %i , %i , %i , %i , %2.4f volts                                         ", S1_p->rec_data, records, S1_p->ibits.in_byte, S1_p->adc_data[0], S1_p->adc_data[1], S1_p->adc_data[2], S1_p->adc_data[3], Vcal0);
			} else {
				snprintf(comm_buffer, 64, "\r\n  B data %x , %i , %i , %i , %i , %i  , %2.4f volts                                             ", S1_p->ibits.in_byte, records, S1_p->adc_data[0], S1_p->adc_data[1], S1_p->adc_data[2], S1_p->adc_data[3], Vcal0);
			}
			S1_p->rec_tmp = SpiStringWrite(comm_buffer);
			if (spi_flag) {
				//				spi_flag = 0;
				SpiStringWrite("\r\n  SRQ received");
			}
		}
		if (SpiSerialReadOk() || !S1_p->ibits.in_bits.eject) {
			S1_p->char_ready = TRUE;
			if ((S1_p->rec_tmp == 'f') || !S1_p->ibits.in_bits.eject) {
				if (S1_p->ibits.in_bits.card_detect) { // no disk
					blink_led(0, LED_ON, FALSE);
					S1_p->obits.out_bits.eject_led = OFF;
					S1_p->ibits.in_byte = SpiPortWrite(S1_p->obits.out_byte);
					SpiStringWrite("\r\n Insert Disk! y/n ");
					while (!SpiSerialReadReady() && S1_p->ibits.in_bits.card_detect) S1_p->ibits.in_byte = SpiPortWrite(S1_p->obits.out_byte);
					V.Timer1 = update_rate;
					while (V.Timer1);
					if (!S1_p->ibits.in_bits.card_detect) {
						SpiStringWrite("/\r\n Mounting Disk ");
						S1_p->obits.out_bits.eject_led = ON;
						S1_p->ibits.in_byte = SpiPortWrite(S1_p->obits.out_byte);
						eresult = f_mount(&FatFs, "0:", 1);
						if (eresult) {
							sprintf(comm_buffer, "\r\n Mount error %i ", eresult);
							SpiStringWrite(comm_buffer); /* Register work area to the logical drive 1 */
							blink_led(GREEN_LED, LED_OFF, FALSE);
							blink_led(RED_LED, LED_ON, FALSE);
						} else {
							Show_MMC_Info();
							blink_led(RED_LED, LED_OFF, FALSE);
							blink_led(GREEN_LED, LED_ON, TRUE);
						}
						/* Create destination file on the drive 1 */
						eresult = f_open(&File[0], "0:logfile.txt", FA_CREATE_ALWAYS | FA_WRITE);
						SpiStringWrite("\r\n Mount Complete ");

						S1_p->ibits.in_byte = SpiPortWrite(S1_p->obits.out_byte);
						if (S1_p->ibits.in_bits.card_detect) SD_NOTRDY = STA_NOINIT;
					}
				} else { // Eject current disk
					blink_led(0, LED_ON, FALSE);
					S1_p->obits.out_bits.eject_led = OFF;
					S1_p->ibits.in_byte = SpiPortWrite(S1_p->obits.out_byte);
					f_close(&File[0]);
					SpiStringWrite("\r\n Eject Disk? y/n ");
					while (!SpiSerialReadReady() && !S1_p->ibits.in_bits.card_detect) S1_p->ibits.in_byte = SpiPortWrite(S1_p->obits.out_byte);
					V.Timer1 = update_rate;
					while (V.Timer1);
					if (S1_p->ibits.in_bits.card_detect) {
						SpiStringWrite("\r\n Ejecting Disk ");
						S1_p->obits.out_bits.eject_led = ON;
						S1_p->ibits.in_byte = SpiPortWrite(S1_p->obits.out_byte);

						if (f_mount(0, "0:", 0)) SpiStringWrite("\r\n UMount error "); /* Unregister work area from the logical drive 1 */
						SpiStringWrite("\r\n Ejection Complete ");
						eresult = 1;

						S1_p->ibits.in_byte = SpiPortWrite(S1_p->obits.out_byte);
						if (S1_p->ibits.in_bits.card_detect) SD_NOTRDY = STA_NOINIT;
					}
				}
			}
		} else {
			S1_p->char_ready = FALSE;
		}

		V.Timer1 = update_rate;
		while (V.Timer1);

		// only GREEN if the data sent and received match or eject button is pressed
		if (S1_p->ibits.in_bits.eject && (S1_p->rec_data != data_mix_old)) {
			//            blink_led(0, LED_ON, TRUE);
		} else {
			//            blink_led(0, LED_ON, FALSE);
		}

		if (!eresult) {
			a = f_puts(comm_buffer, &File[0]);
			blink_led(RED_LED, LED_OFF, FALSE);
			blink_led(GREEN_LED, LED_ON, TRUE);
			records++;
		} else {
			file_errors++;
			if ((file_errors % 100) == 0) {
				//				SpiStringWrite("\r\n not logged ");
				blink_led(GREEN_LED, LED_OFF, FALSE);
				blink_led(RED_LED, LED_ON, FALSE);
			}
		}

		if ((a == (-1))&&(eresult != 1)) {
			if ((file_errors % 100) == 0) {
				sprintf(comm_buffer, "\r\n File Write error %i ", a);
				SpiStringWrite(comm_buffer);
				blink_led(GREEN_LED, LED_OFF, FALSE);
				blink_led(RED_LED, LED_ON, FALSE);
			}
		}
		f_sync(&File[0]);
	}
}

