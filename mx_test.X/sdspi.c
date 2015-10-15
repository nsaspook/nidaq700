/* SPI interface to SDCARD */
#include "mx_test.h"
#include "sdspi.h"
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "src/diskio.h"

#ifdef SDEBUG
#include <usart.h>
#define SDEBUGM(msg)  P2wait(); TXREG2 = msg
#else
#define SDEBUGM(msg)
#endif

#define	BUS_CHAN	SPI_CHANNEL2
#define SDCARD_CHAN	SPI_CHANNEL1


/* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC 0x01 /* MMC ver 3 */
#define CT_SD1 0x02 /* SD ver 1 */
#define CT_SD2 0x04 /* SD ver 2 */
#define CT_SDC (CT_SD1|CT_SD2) /* SD */
#define CT_BLOCK 0x08 /* Block addressing */

static int valid_rec_char = FALSE;
static UINT CardType;
volatile static struct timeval current_time;

/* 
 * branch macros for MIPS 
 */
#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */

/*-----------------------------------------------------------------------*/

static int wait_ready(void)
{
	BYTE d;

	V.Timer3 = 500; /* Wait for ready in timeout of 500ms */
	do {
		d = xmit_spi_sdcard(0xFF);
	} while ((d != 0xFF) && V.Timer3);

	return(d == 0xFF) ? 1 : 0;
}

void sd_deselect(void)
{
	mPORTBSetBits(BIT_2); // deselect
	xmit_spi_sdcard(0xFF); /* Dummy clock (force DO hi-z for multiple slave SPI) */
}

/*
 * return TRUE on select (proper response to dummies from card)
 */
int sd_select(void)
{
	mPORTBClearBits(BIT_2); // select
	xmit_spi_sdcard(0xFF); /* Dummy clock (force DO enabled) */

	if (wait_ready()) {
		return 1; /* OK */
	} else {
		sd_deselect(); /* bad response, no card? */
		return 0; /* Timeout */
	}
}

/*
 *  B3 is the slave (de)select bit for the address decoder 74HC138
 *  B0|B1 are the address bits for the slave [0..2] for 4 possible devices on the bus
 */
void ps_deselect(void)
{
	mPORTBSetBits(BIT_3); // deselect
	mPORTBSetBits(BIT_0 | BIT_1); // set address bits high for 7
}

void ps_select(int ps_id)
{
	mPORTASetBits(BIT_3); // deselect move cs signal out of the device range
	mPORTBSetBits(BIT_0 | BIT_1); // set address
	switch (ps_id) {
	case 0:
		mPORTBClearBits(BIT_0 | BIT_1); // set address
		break;
	case 1:
		mPORTBClearBits(BIT_1); // set address
		mPORTBSetBits(BIT_0); // set address
		break;
	case 2:
		mPORTBClearBits(BIT_0); // set address
		mPORTBSetBits(BIT_1); // set address
		break;
	case 3:
		mPORTBSetBits(BIT_0 | BIT_1); // set address
		break;
	default:
		return;
	}
	mPORTBClearBits(BIT_3); // clear enable bit to move cs bit into the device range
}

/*
 * CRC7 for commands Supported in SPI mode with an argument of 0 "stuff bits only"
 * CRC7 (As an 8bit value with bit zero "End bit" set)
 * Command (As an 8 bit value with bit 6 "transmission *bit" set)

Command.........Abbreviation............CRC7
CMD0............GO_IDLE_STATE...........0x95
CMD9............SEND_CSD................0xAf
CMD10...........SEND_CID................0x1B
CMD12...........STOP_TRANSMISSION.......0x61
CMD13...........SEND_STATUS.............0x0D
CMD27...........PROGRAM_CSD.............0xDB
CMD38...........ERASE...................0xA5
CMD55...........APP_CMD.................0x65
CMD58...........READ_OCR................0xFD

ACMD51..........SEND_SCR................0xC7
 */

unsigned int CRC16(unsigned char *data, unsigned int length)
{
	static unsigned int crc = 0;

	crc = 0;
	while (length--) {
		crc = (unsigned char) (crc >> 8) | (crc << 8);
		crc ^= *data++;
		crc ^= (unsigned char) (crc & 0xff) >> 4;
		crc ^= (crc << 8) << 4;
		crc ^= ((crc & 0xff) << 4) << 1;
	}

	return( crc);
}

/*
 *  delay a short time after spi command or exit on slave SRQ signal if srq is true
 *  flag will always be unset to low on exit
 */

static void DelaySPI(WORD delay, int srq)
{
	unsigned int int_status;

	while (delay--) {
		if (likely(srq & spi_flag)) // make the instruction cache preload the exit code
			goto DelaySPI_exit;
		int_status = INTDisableInterrupts();
		OpenCoreTimer(500000 / 2000);
		INTRestoreInterrupts(int_status);
		mCTClearIntFlag();
		//				while (!mCTGetIntFlag()); // timeout
		while (!mCTGetIntFlag() && !spi_flag); // timeout or SRQ flag set
	}
DelaySPI_exit:
	mCTClearIntFlag();
	spi_flag = LOW;
}

/*-----------------------------------------------------------------------*/
/* Transmit a byte via SPI2  (Platform dependent)                  */

/*-----------------------------------------------------------------------*/

unsigned char xmit_spi_bus(unsigned char dat, WORD pace, WORD srq)
{
	unsigned char rec_buffer;

	V.spi_count++;
	SpiChnPutC(BUS_CHAN, dat); // Send data on the master channel, SPI2
	while (SpiChnIsBusy(BUS_CHAN));
	rec_buffer = SpiChnGetC(BUS_CHAN); // Get the received data
	DelaySPI(pace, srq);
	return rec_buffer; // Get the received data
}


/*-----------------------------------------------------------------------*/
/* Receive a byte via SPI2  (Platform dependent)                 */

/*-----------------------------------------------------------------------*/

unsigned char rcvr_spi_bus(void)
{
	V.spi_count++;
	return xmit_spi_bus(0xff, 0, HIGH);
}

/*-----------------------------------------------------------------------*/
/* Transmit a byte to MMC/SD via SPI1  (Platform dependent)                  */

/*-----------------------------------------------------------------------*/

unsigned char xmit_spi_sdcard(unsigned char dat)
{
	static unsigned char rec_buffer;
	V.card_count++;
	SpiChnPutC(SDCARD_CHAN, dat); // Send data on the master channel, SPI1
	while (SpiChnIsBusy(SDCARD_CHAN));
	DelaySPI(1, LOW);
	rec_buffer = SpiChnGetC(SDCARD_CHAN); // Get the received data
	DelaySPI(1, LOW);
	return rec_buffer;
}


/*-----------------------------------------------------------------------*/
/* Receive a byte from MMC via SPI  (Platform dependent)                 */

/*-----------------------------------------------------------------------*/

unsigned char rcvr_spi_sdcard(void)
{
	V.card_count++;
	return xmit_spi_sdcard(0xff);
}


/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC/SD Card                                          */

/*-----------------------------------------------------------------------*/

unsigned char send_cmd(unsigned char cmd, /* Command byte */
	unsigned long arg /* Argument */
	)
{
	unsigned char n, res;

	if (cmd & 0x80) { /* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > (unsigned char) 1)
			return res;
	}

	sd_deselect();
	if (!sd_select()) return 0xff;

	/* Send command packet */
	xmit_spi_sdcard(cmd); /* Command */
	xmit_spi_sdcard((unsigned char) (arg >> 24)); /* Argument[31..24] */
	xmit_spi_sdcard((unsigned char) (arg >> 16)); /* Argument[23..16] */
	xmit_spi_sdcard((unsigned char) (arg >> 8)); /* Argument[15..8] */
	xmit_spi_sdcard((unsigned char) arg); /* Argument[7..0] */
	n = (unsigned char) 0xFF; // fake crc7
	if (cmd == (unsigned char) CMD0)
		n = 0x95; /* CRC for CMD0(0) */
	if (cmd == (unsigned char) CMD8)
		n = 0x87; /* CRC for CMD8(0x1AA) */
	if (cmd == (unsigned char) CRC_ON_OFF)
		n = 0xAD;
	xmit_spi_sdcard(n);

	/* Receive command response */
	if (cmd == (unsigned char) CMD12)
		rcvr_spi_sdcard(); /* Skip a stuff byte when stop reading */
	n = 10; /* Wait for a valid response in timeout of 10 attempts */
	do
		res = rcvr_spi_sdcard(); while ((res & 0x80) && --n);
	return res; /* Return with the response value */
}

void send_dummys(void)
{
	unsigned char n;
	for (n = 10; n; n--)
		xmit_spi_sdcard(0xff); /* 80 dummy clocks */
}

int MMC_get_volume_info(void)
//****************************************************************************
{
	SDC0.sddetect = FALSE;
	if (!SDC0.sdinit) {
		return ERR1; // no card after second try
	}
	vinf = &MMC_volume_Info; //Init the pointer;
	if (SDC0.sdtype == SDT6) { // SDHC size calc
		vinf->sector_count = (((unsigned long) csd[7] & 0x3F) << 16) | ((unsigned long) csd[8] << 8) | (unsigned long) csd[9];
		vinf->size_MB = (vinf->sector_count + 1) * SDBUFFERSIZE;
	} else { // SD/MMC size calc
		vinf->read_block_len = (unsigned long) 1 << (csd[5] & 0x0F); // sector size

		vinf->sector_count = (unsigned long) csd[6] & 0x03;
		vinf->sector_count <<= SHIFT8;
		vinf->sector_count += (unsigned long) csd[7];
		vinf->sector_count <<= SHIFT2;
		vinf->sector_count += (unsigned long) ((unsigned long) csd[8] & 0xc0) >> 6;

		vinf->sector_multiply = (unsigned long) csd[9] & 0x03;
		vinf->sector_multiply <<= SHIFT1;
		vinf->sector_multiply += (unsigned long) ((unsigned long) csd[10] & 0x80) >> 7;
		// work out the MBs
		// mega bytes in u08 == C_SIZE / (2^(9-C_SIZE_MULT))
		vinf->size_MB = (vinf->sector_count >> (9 - vinf->sector_multiply)) * (vinf->read_block_len / SDBUFFERSIZE);
	}

	vinf->serial = ((unsigned long) cid[9] << 24) + ((unsigned long) cid[10] << 16) + ((unsigned long) cid[11] << 8) + (unsigned long) cid[12]; // Serial Number
	// get the name of the card
	vinf->name[0] = cid[3];
	vinf->name[1] = cid[4];
	vinf->name[2] = cid[5];
	vinf->name[3] = cid[6];
	vinf->name[4] = cid[7];
	vinf->name[5] = 0x00; //end flag

	SDC0.sddetect = TRUE;
	return 0;
}

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */

/*-----------------------------------------------------------------------*/

int MMC_disk_initialize()
{
	static unsigned char n, cmd, i;

	SD_NOTRDY = STA_NOINIT;
	SDC0.sdinit = FALSE;
	SpiChnSetBrg(SDCARD_CHAN, 64); // set SCK to slow speed
	V.Timer4 = 5;
	while (V.Timer4); // wait for power to settle
	sd_deselect();
	send_dummys(); /* 80 dummy clocks */
	sd_select();
	V.Timer4 = 5;
	SDC0.sdtype = 0;

	if (send_cmd(CMD0, 0) == (unsigned char) HIGH) { /* Enter Idle state */
		if (send_cmd(CMD8, 0x1AA) == (unsigned char) HIGH) { /* SDC Ver2+ */
			for (n = 0; n < (unsigned char) 4; n++) {
				ocr[n] = rcvr_spi_sdcard(); /* Get trailing data of R7 resp */
			}
			if ((ocr[2] == (unsigned char) 0x01) && (ocr[3] == (unsigned char) 0xAA)) { /* The card can work at vdd range of 2.7-3.6V */
				send_cmd(CRC_ON_OFF, 0); // no checking CRC, redundant
				while (send_cmd(ACMD41, 1UL << 30)); /* ACMD41 with HCS bit */
				V.Timer4 = 1500;
				while (V.Timer4); // wait a long time for card to init
				if (send_cmd(CMD58, 0) == (unsigned char) 0) { /* Check CCS bit in the OCR */
					for (n = 0; n < (unsigned char) 4; n++)
						ocr[n] = rcvr_spi_sdcard();
					SDC0.sdtype = (ocr[0] & 0x40) ? SDT6 : SDT2;
					CardType = SDC0.sdtype;
				}
			}
		} else { /* SDC Ver1 or MMC */
			if (send_cmd(ACMD41, 0) <= (unsigned char) 1) {
				SDC0.sdtype = SDT2;
				cmd = ACMD41; /* SDC */
			} else {
				SDC0.sdtype = SDT1;
				cmd = CMD1; /* MMC */
			}
			CardType = SDC0.sdtype;
			while (send_cmd(cmd, 0)); /* Wait for leaving idle state */

			if (send_cmd(CMD16, SDBUFFERSIZE) == (unsigned char) 0); /* Select R/W block length */
		}
		if (send_cmd(CMD9, 0) == (unsigned char) 0) { // get CSD
			for (i = 0; i < (unsigned char) 16; i++)
				if (rcvr_spi_sdcard() == 0xFE)
					break; // cheat, got data token
			for (n = 0; n < (unsigned char) 16; n++)
				csd[n] = rcvr_spi_sdcard();

		}
		if (send_cmd(CMD10, 0) == (unsigned char) 0) { // get CID
			for (i = 0; i < (unsigned char) 16; i++)
				if (rcvr_spi_sdcard() == 0xFE)
					break; // cheat, got data token
			for (n = 0; n < (unsigned char) 16; n++)
				cid[n] = rcvr_spi_sdcard();
		}
		sd_deselect();
		//		send_dummys(); /* 80 dummy clocks */
	} else {
		sd_deselect();
		return ERR1; //      no card
	}
	if (SDC0.sdtype != SDT6) {
		sd_deselect();
		SpiStringWrite("\r\n SDHC Cards Only \r\n");
		return ERR2; //      wrong type card
	}
	SDC0.sdinit = TRUE;
	SD_NOTRDY = RES_OK;
	SpiChnSetBrg(SDCARD_CHAN, 4); // set SCK to high speed
	MMC_get_volume_info();
	return RES_OK;
}

int MMC_disk_status(void)
{
	return SD_NOTRDY; // return disk status variable
}

int MMC_disk_write(const BYTE* buff, DWORD sector, UINT count)
{
	UINT result;

	result = mmc_write_block(buff, sector);
	return result;
}

/************************** MMC/SHDC Write Block **************************************/

int mmc_write_block(const BYTE* buff, unsigned long block_number)
{
	static int retry, i, nsec, ssm;
	static unsigned char tmp;
	static unsigned long block_tmp;

	SDEBUGM('W');
	block_tmp = block_number;
	if (SDC0.sdtype != SDT6) { // use byte style blocks for SD cards
		block_tmp = block_tmp << SHIFT9;
	}

	retry = 0;
	do {
		if (SDC0.sdtype == SDT6) {
			tmp = send_cmd(CMD24, block_tmp); // send sdhc write block, arg is block number
			SDEBUGM('6');
		} else {
			tmp = send_cmd(CMD25, block_tmp); // send sd multi write block, arg is block number
			SDEBUGM('2');
		}
		retry++;
		if (retry == 100) {
			SDEBUGM('E');
			return ERR1; // write command error
		}
	} while (tmp != (unsigned char) 0);
	send_dummys();

	if (SDC0.sdtype == SDT6) {
		xmit_spi_sdcard(0xFE); // send single block token

		for (i = 0; i < SDBUFFERSIZE; i++) {
			xmit_spi_sdcard(buff[i]); // send data to card
		}
		xmit_spi_sdcard(0xFF); // dummy CRC16
		xmit_spi_sdcard(0xFF);
		if ((rcvr_spi_sdcard() & 0x1f) != (unsigned char) 0x05) {
			SDEBUGM('E');
			return ERR1;
		}
		while (rcvr_spi_sdcard() != 0xff); // wait
		sd_deselect(); // set SS = 1 (off)
		SDEBUGM('w');
		return 0;
	} else {
		ssm = vinf->read_block_len / SDBUFFERSIZE;
		for (nsec = 1; nsec <= ssm; nsec++) {
			xmit_spi_sdcard(0b11111100); // send multi block token
			for (i = 0; i < SDBUFFERSIZE; i++) {
				xmit_spi_sdcard(buff[i]); // send data to card
			}
			xmit_spi_sdcard(0xFF); // dummy CRC16
			xmit_spi_sdcard(0xFF);
			if ((rcvr_spi_sdcard() & 0x1f) != (unsigned char) 0x05) {
				SDEBUGM('E');
				return ERR1;
			}
			while (rcvr_spi_sdcard() != 0xff); // wait
		}
		xmit_spi_sdcard(0b11111101); // stop tran token
		while (rcvr_spi_sdcard() != 0xff); // wait
		sd_deselect(); // set SS = 1 (off)
		SDEBUGM('w');
		return 0;
	}
}

int MMC_disk_read(BYTE* buff, DWORD sector, UINT count)
{
	UINT result;

	result = mmc_read_block(buff, sector);
	return result;
}

/************************** MMC/SDHC Read Block **************************************/

/**** Reads a SDBUFFERSIZE Byte block from the MMC  ****/

int mmc_read_block(BYTE* buff, unsigned long block_number)
{
	static int retry, i, nsec, ssm;
	static unsigned char tmp;
	static unsigned long block_tmp;

	SDEBUGM('R');
	block_tmp = block_number;
	if (SDC0.sdtype != SDT6) { // use byte style blocks for SD cards
		block_tmp = block_tmp << SHIFT9;
	}

	retry = 0;
	do {
		if (SDC0.sdtype == SDT6) {
			tmp = send_cmd(CMD17, block_tmp); // send sdhc read single block command
			SDEBUGM('6');
		} else {
			tmp = send_cmd(CMD18, block_tmp); // send sdhc read single block command
			SDEBUGM('2');
		}
		retry++;
		if (retry == 1000) {
			SDEBUGM('E');
			return ERR1; // write command error
		}
	} while (tmp != (unsigned char) 0);

	retry = 0;
	do {
		tmp = rcvr_spi_sdcard();
		retry++;
		if (retry == 1000) {
			SDEBUGM('E');
			return ERR1; // receive command error
		}
	} while (tmp != (unsigned char) 0xfe); // got data token

	if (SDC0.sdtype == SDT6) {
		for (i = 0; i < SDBUFFERSIZE; i++) {
			buff[i] = rcvr_spi_sdcard(); // send data to card
		}
		rcvr_spi_sdcard(); // CRC16 bytes that are not needed
		rcvr_spi_sdcard();
		sd_deselect(); // set SS = 1 (off)
		SDEBUGM('r');
		return 0;
	} else {
		ssm = vinf->read_block_len / SDBUFFERSIZE;
		for (nsec = 1; nsec <= ssm; nsec++) {
			for (i = 0; i < SDBUFFERSIZE; i++) {
				buff[i] = rcvr_spi_sdcard(); // send data to card
			}
			rcvr_spi_sdcard(); // CRC16 bytes that are not needed
			rcvr_spi_sdcard();
		}
		// send stop tran
		tmp = send_cmd(CMD12, block_tmp); // send sdhc read single block command
		rcvr_spi_sdcard();
		while (rcvr_spi_sdcard() == (unsigned char) 0); // wait
		sd_deselect(); // set SS = 1 (off)
		SDEBUGM('r');
		return 0;
	}
}

unsigned char SpiStringWrite(char* data)
{
	unsigned int i, len, ret_char, tmp_char;

	ps_select(0);
	len = strlen(data);
	if (len > MAXSTRLEN) len = MAXSTRLEN - 2;
	if (len) {
		ret_char = SpiSerialWrite(data[0]);
		if (SpiSerialReadReady()) { // ready status
			valid_rec_char = TRUE;
		}
		DelaySPI(52, LOW);
		for (i = 1; i <= len; i++) {
			tmp_char = SpiSerialWrite(data[i]);
			if (SpiSerialReadReady()) { // ready status
				valid_rec_char = TRUE;
				ret_char = tmp_char;
			}
			DelaySPI(52, LOW);
		}
		return ret_char;
	}
	return 0;
}

unsigned int SpiIOPoll(unsigned int lamp)
{
	unsigned int p_switch[3];
	/*
	 * Need a delay for remote SPI processing
	 */
	ps_select(1);
	spi_flag = LOW; // reset the SRQ flag
	p_switch[0] = xmit_spi_bus(SPI_CMD_RW, 1, LOW);
	p_switch[1] = xmit_spi_bus(lamp, 1, LOW);
	p_switch[2] = xmit_spi_bus(lamp, 1, LOW);
	return p_switch[1]+(p_switch[2] << 8);
}

int SpiADCRead(unsigned char channel)
{
	ps_select(0);
	V.adc_count++;
	xmit_spi_bus(CMD_DUMMY_CFG, 1, HIGH);
	spi_flag = HIGH; // don't wait
	cmd_response_port = xmit_spi_bus(CMD_ADC_GO_H | (channel & 0x0f), 1, HIGH);
	DelaySPI(50, HIGH); // delay for adc conversion time and look for SRQ signal
	cmd_data[0] = xmit_spi_bus(CMD_ADC_DATA, 1, HIGH);
	cmd_data[1] = xmit_spi_bus(CMD_DUMMY_CFG, 1, HIGH);
	return(short int) (cmd_data[0] | (cmd_data[1] << 8)); /* use the short to make this a signed type */
}

unsigned char SpiPortWrite(unsigned char data)
{
	ps_select(0);
	V.data_count++;
	xmit_spi_bus(CMD_DUMMY_CFG, 1, HIGH);
	cmd_response_port = xmit_spi_bus(CMD_PORT_GO | (data & 0x0f), 1, HIGH);
	cmd_data[1] = xmit_spi_bus(CMD_PORT_DATA | ((data >> 4) &0x0f), 1, HIGH);
	return cmd_data[1];
}

unsigned char SpiSerialWrite(unsigned char data)
{
	ps_select(0);
	V.char_count++;
	xmit_spi_bus(CMD_DUMMY_CFG, 1, HIGH);
	cmd_response_char = xmit_spi_bus(CMD_CHAR_GO | (data & 0x0f), 1, HIGH);
	cmd_data[0] = xmit_spi_bus(CMD_CHAR_DATA | ((data >> 4) &0x0f), 1, HIGH);
	return cmd_data[0];
}

int SpiSerialReadOk(void)
{
	ps_select(0);
	if (valid_rec_char) {
		valid_rec_char = FALSE;
		return TRUE;
	}
	return FALSE;
}

int SpiSerialReadReady(void)
{
	ps_select(0);
	cmd_response_char = xmit_spi_bus(CMD_DUMMY_CFG, 1, HIGH);
	if (cmd_response_char & UART_DUMMY_MASK) return TRUE;
	return FALSE;
}

unsigned char SpiSerialGetChar(void)
{
	ps_select(0);
	cmd_response_char = xmit_spi_bus(CMD_CHAR_RX, 1, HIGH);
	return xmit_spi_bus(CMD_DUMMY_CFG, 1, HIGH);
}

int SpiStatus(void)
{
	ps_select(0);
	cmd_response_char = xmit_spi_bus(CMD_DUMMY_CFG, 1, HIGH);
	if (cmd_response_char != 0xff) return TRUE;
	return FALSE;
}

void SpiInitDevice1(SpiChannel chn, int speed)
{
	SpiOpenFlags oFlags = SPI_OPEN_MODE8 | SPI_OPEN_MSTEN | SPI_OPEN_CKE_REV; // SPI open mode
	// Open SPI module, use SPI channel , use flags set above, Divide Fpb by X
	SpiChnOpen(chn, oFlags, speed);
}

void SpiInitDevice2(SpiChannel chn, int speed)
{
	SpiOpenFlags oFlags = SPI_OPEN_MODE8 | SPI_OPEN_MSTEN | SPI_OPEN_CKE_REV; // SPI open mode
	// Open SPI module, use SPI channel , use flags set above, Divide Fpb by X
	SpiChnOpen(chn, oFlags, speed);
}

void init_spi_ports(void)
{
	SpiInitDevice1(SDCARD_CHAN, 16); // Initialize the SPI channel 1 as master. slow SCK for SDCARD init

	SpiInitDevice2(BUS_CHAN, 2); // Initialize the SPI channel 2 as master, 2.083 mhz SCK
	ps_select(0);
}

DRESULT MMC_disk_ioctl(BYTE cmd, void *buff)
{
	DRESULT res;
	BYTE n, *ptr = buff;
	DWORD csz;

	if (SD_NOTRDY) return RES_NOTRDY;

	res = RES_ERROR;
	switch (cmd) {
	case CTRL_SYNC: /* Flush write-back cache, Wait for end of internal process */
		if (!SD_NOTRDY) res = RES_OK;
		break;

	case GET_SECTOR_COUNT: /* Get number of sectors on the disk (WORD) */
		if (!SD_NOTRDY) {
			if ((csd[0] >> 6) == 1) { /* SDv2? */
				csz = csd[9] + ((WORD) csd[8] << 8) + ((DWORD) (csd[7] & 63) << 16) + 1;
				*(DWORD*) buff = csz << 10;
			} else { /* SDv1 or MMCv3 */
				n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
				csz = (csd[8] >> 6) + ((WORD) csd[7] << 2) + ((WORD) (csd[6] & 3) << 10) + 1;
				*(DWORD*) buff = csz << (n - 9);
			}

			res = RES_OK;
		}
		break;

	case GET_BLOCK_SIZE: /* Get erase block size in unit of sectors (DWORD) */
		if (CardType & CT_SD2) { /* SDv2? */
			if (send_cmd(ACMD13, 0) == 0) { /* Read SD status */
				xmit_spi_sdcard(0xFF);
				if (!SD_NOTRDY) { /* Read partial block */
					for (n = 64 - 16; n; n--) xmit_spi_sdcard(0xFF); /* Purge trailing data */
					*(DWORD*) buff = 16UL << (csd[10] >> 4);
					res = RES_OK;
				}
			}
		} else { /* SDv1 or MMCv3 */
			if (!SD_NOTRDY) { /* Read CSD */
				if (CardType & CT_SD1) { /* SDv1 */
					*(DWORD*) buff = (((csd[10] & 63) << 1) + ((WORD) (csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
				} else { /* MMCv3 */
					*(DWORD*) buff = ((WORD) ((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
				}
				res = RES_OK;
			}
		}
		break;

	case MMC_GET_TYPE: /* Get card type flags (1 byte) */
		*ptr = CardType;
		res = RES_OK;
		break;

	case MMC_GET_CSD: /* Receive CSD as a data block (16 bytes) */
		if (!SD_NOTRDY) { /* READ_CSD */
			buff = csd;
			res = RES_OK;
		}
		break;

	case MMC_GET_CID: /* Receive CID as a data block (16 bytes) */
		if (!SD_NOTRDY) { /* READ_CID */
			buff = cid;
			res = RES_OK;
		}
		break;

	case MMC_GET_OCR: /* Receive OCR as an R3 resp (4 bytes) */
		if (send_cmd(CMD58, 0) == 0) { /* READ_OCR */
			for (n = 0; n < 4; n++)
				*((BYTE*) buff + n) = xmit_spi_sdcard(0xFF);
			res = RES_OK;
		}
		break;

	case MMC_GET_SDSTAT: /* Receive SD statsu as a data block (64 bytes) */
		if (!SD_NOTRDY) { /* SD_STATUS */
			res = RES_OK;
		}
		break;

	default:
		res = RES_PARERR;
	}

	return res;
}

// Software 1ms timers

void disk_timerproc(void)
{
	UINT n;

	n = V.Timer1; /* 1000Hz decrement timer with zero stopped */
	if (n) V.Timer1 = --n;
	n = V.Timer2;
	if (n) V.Timer2 = --n;
	n = V.Timer3;
	if (n) V.Timer3 = --n;
	n = V.Timer4;
	if (n) V.Timer4 = --n;
	V.timerint_count++;
}

DWORD get_fattime(void)
{
	time_t CurrentTimeStamp;
	struct tm *CurrentLocalTime;
	DWORD FATFSTimeCode;

	CurrentTimeStamp = time(NULL);
	CurrentLocalTime = localtime(&CurrentTimeStamp);

	//Map the tm struct time into the FatFs time code
	FATFSTimeCode = ((CurrentLocalTime->tm_year - 80) << 25) |
		((CurrentLocalTime->tm_mon + 1) << 21) |
		((CurrentLocalTime->tm_mday) << 16) |
		((CurrentLocalTime->tm_hour) << 11) |
		((CurrentLocalTime->tm_min) << 5) |
		((CurrentLocalTime->tm_sec));

	return FATFSTimeCode;
}

void RTC_Init()
{
	current_time.tv_sec = 0;
	current_time.tv_usec = 0;
	// High priority interrupt for RTC timer. Called 1000/sec.
	ConfigIntTimer5(T5_INT_ON | T5_INT_PRIOR_7);
	OpenTimer5(T5_ON | T5_SOURCE_INT | T5_PS_1_8, (50000000 / 2 / 64 / 1000)); // for 1khz
}

void __ISR(_TIMER_5_VECTOR, IPL7AUTO) TimerRTCHandler(void)
{
	current_time.tv_usec++;
	if (current_time.tv_usec == 1000) {
		current_time.tv_sec++;
		current_time.tv_usec = 0;
	}
	disk_timerproc(); // sdcard ff time process
	V.timerint_count++;
	INTClearFlag(INT_T5);
}

int gettimeofday(struct timeval *tv, void *tz)
{
	unsigned int intStatus;
	intStatus = INTDisableInterrupts();
	tv->tv_usec = current_time.tv_usec;
	tv->tv_sec = current_time.tv_sec;
	INTRestoreInterrupts(intStatus);
	return 0;
}

int settimeofday(const struct timeval *tv, void *tz)
{
	unsigned int intStatus;
	intStatus = INTDisableInterrupts();
	current_time.tv_usec = tv->tv_usec;
	current_time.tv_sec = tv->tv_sec;
	INTRestoreInterrupts(intStatus);
	return 0;
}

time_t time(time_t *tod)
{
	time_t t;
	t = (time_t) current_time.tv_sec;
	if (tod != NULL) *tod = t;
	return t;
}



