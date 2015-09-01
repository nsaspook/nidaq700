/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */

#include "../sdspi.h"		/* Example: MMC/SDC contorl */

#include <stdio.h>

/* Definitions of physical drive number for each media */
#define ATA		0
#define MMC		1
#define USB		2

// static char error_string[32];
/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
	BYTE pu /* Physical drive nmuber (0..) */
	)
{
	int result;

	result = MMC_disk_initialize();
//	sprintf(error_string, "\r\n disk_initialize error %i ", result);
//	if (result) SpiStringWrite(error_string);
	// translate the result code here

	return result;

}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */

/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
	BYTE pdrv /* Physical drive nmuber (0..) */
	)
{
	int result;


	result = MMC_disk_status();
//	sprintf(error_string, "\r\n disk_status error %i ", result);
//	if (result) SpiStringWrite(error_string);
	// translate the result code here

	return result;


}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

/*-----------------------------------------------------------------------*/

DRESULT disk_read(
	BYTE pdrv, /* Physical drive nmuber (0..) */
	BYTE *buff, /* Data buffer to store read data */
	DWORD sector, /* Sector address (LBA) */
	UINT count /* Number of sectors to read (1..128) */
	)
{
	int result;


	// translate the arguments here

	result = MMC_disk_read(buff, sector, count);
//	sprintf(error_string, "\r\n disk_read error %i ", result);
//	if (result) SpiStringWrite(error_string);
	// translate the reslut code here

	return result;


}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE

DRESULT disk_write(
	BYTE pdrv, /* Physical drive nmuber (0..) */
	const BYTE *buff, /* Data to be written */
	DWORD sector, /* Sector address (LBA) */
	UINT count /* Number of sectors to write (1..128) */
	)
{
	int result;


	// translate the arguments here

	result = MMC_disk_write(buff, sector, count);
//	if (result) SpiStringWrite("\r\n Disk_write ");
	// translate the reslut code here

	return result;


}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL

DRESULT disk_ioctl(
	BYTE pdrv, /* Physical drive nmuber (0..) */
	BYTE cmd, /* Control code */
	void *buff /* Buffer to send/receive control data */
	)
{
	int result;


	// pre-process here
	MMC_get_volume_info();
	result = MMC_disk_ioctl(cmd, buff);
//	if (result) SpiStringWrite("\r\n disk_ioctl ");
	// post-process here

	return result;


}
#endif
