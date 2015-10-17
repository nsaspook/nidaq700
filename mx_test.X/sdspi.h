#ifndef SDSPI_H_INCLUDED
#define SDSPI_H_INCLUDED

#include "mx_test_defs.h"
#include "mx_test_types.h"
#include "src/diskio.h"

void send_dummys(void); // Send 80 clocks to SD card
int MMC_disk_initialize(void); // Check for card and init via SPI
int MMC_disk_status(void); // card mounted and ready
int MMC_disk_read(BYTE*, DWORD, UINT);
int MMC_disk_write(const BYTE*, DWORD, UINT);
void MM_state(int);
unsigned char MM_detect(void);
DRESULT MMC_disk_ioctl(BYTE, void*);
void disk_timerproc(void);
DWORD get_fattime(void);
void RTC_Init();
int MMC_get_volume_info(void); // Read card info
void init_spi_ports(void); // open spi ports and config
unsigned char xmit_spi_bus(unsigned char, WORD, WORD); // Send 1 byte to SPI2 and delay or wait for SRQ if needed
unsigned char rcvr_spi_bus(void); // Receive 1 byte from SPI2
unsigned char xmit_spi_sdcard(unsigned char); // Send 1 byte to card, SPI1
unsigned char rcvr_spi_sdcard(void); // Receive 1 byte from card, SPI1
int mmc_write_block(const BYTE*, unsigned long); // Write SDBUFFERSIZE bytes to card at block address (SDHC style)
int mmc_read_block(BYTE*, unsigned long); // Read SDBUFFERSIZE bytes from card at block address (SHDC style)
void ps_deselect(void);
void ps_select(int);
int SpiSerialReadReady(void);
int SpiSerialReadOk(void);
unsigned char SpiSerialWrite(unsigned char);
unsigned char SpiPortWrite(unsigned char);
unsigned int SpiIOPoll(unsigned int);
int SpiADCRead(unsigned char);
unsigned char SpiStringWrite(char*);
unsigned char SpiSerialGetChar(void);

extern VOLUME_INFO_TYPE *vinf;
extern void wdttime(unsigned long int);
extern volatile struct V_data V;
extern union Obits2 LEDS;

#endif /* SDSPI_H_INCLUDED */

