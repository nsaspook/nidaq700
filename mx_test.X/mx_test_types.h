/* 
 * File:   mx_test_types.h
 * Author: root
 *
 * Created on October 22, 2013, 2:02 PM
 */

#ifndef MX_TEST_TYPES_H
#define	MX_TEST_TYPES_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "mx_test_defs.h"

#ifdef INTTYPES
#include <stdint.h>
#else
#define INTTYPES
	/*unsigned types*/
	typedef unsigned char uint8_t;
	typedef unsigned short int uint16_t;
	typedef unsigned long uint32_t;
	typedef unsigned long long uint64_t;
	/*signed types*/
	typedef signed char int8_t;
	typedef signed short int int16_t;
	typedef signed long int32_t;
	typedef signed long long int64_t;
#endif

	typedef struct SD_VOLUME_INFO {
		uint32_t size_MB;
		uint32_t sector_multiply, read_block_len;
		uint32_t sector_count, serial;
		uint8_t name[SDNAME_SIZE];
	} VOLUME_INFO_TYPE;

	typedef struct SDCARD {
		uint16_t magic;
		uint8_t sdtype, sdinit, sddetect;
	} SDCARD_TYPE;

	typedef struct INBITS1 {
		uint8_t eject : 1;
		uint8_t b1 : 1;
		uint8_t b2 : 1;
		uint8_t b3 : 1;
		uint8_t b4 : 1;
		uint8_t card_detect : 1;
		uint8_t b6 : 1;
		uint8_t b7 : 1;
	} INBITS_TYPE1;

	typedef struct OUTBITS1 {
		uint8_t b0 : 1;
		uint8_t b1 : 1;
		uint8_t eject_led : 1;
		uint8_t b3 : 1;
		uint8_t b4 : 1;
		uint8_t b5 : 1;
		uint8_t b6 : 1;
		uint8_t b7 : 1;
	} OUTBITS_TYPE1;

	typedef struct INBITS2 {
		uint8_t eject : 1;
		uint8_t b1 : 1;
		uint8_t b2 : 1;
		uint8_t b3 : 1;
		uint8_t b4 : 1;
		uint8_t b5 : 1;
		uint8_t b6 : 1;
		uint8_t b7 : 1;
	} INBITS_TYPE2;

	typedef struct OUTBITS2 {
		uint8_t b0 : 1;
		uint8_t b1 : 1;
		uint8_t b2 : 1;
		uint8_t b3 : 1;
		uint8_t b4 : 1;
		uint8_t b5 : 1;
		uint8_t b6 : 1;
		uint8_t b7 : 1;
	} OUTBITS_TYPE2;

	union Ibits1 {
		unsigned char in_byte;
		INBITS_TYPE1 in_bits;
	};

	union Obits1 {
		unsigned char out_byte;
		OUTBITS_TYPE1 out_bits;
	};

	union Ibits2 {
		unsigned char in_byte;
		INBITS_TYPE2 in_bits;
	};

	union Obits2 {
		unsigned char out_byte;
		OUTBITS_TYPE2 out_bits;
	};

	typedef struct SDEV1 {
		uint8_t config, rec_data, rec_tmp, status;
		int adc_data[10], char_ready, chan_no;
		union Ibits1 ibits;
		union Obits1 obits;
	} SDEV_TYPE1;

	typedef struct SDEV2 {
		uint8_t config, rec_data, rec_tmp, status;
		int adc_data[8], char_ready, chan_no;
		union Ibits2 ibits;
		union Obits2 obits;
	} SDEV_TYPE2;

	typedef struct V_data { // ISR used, mainly for non-atomic mod problems
		uint32_t spi_count, timerint_count, char_count, card_count;
		uint32_t data_count, blink_count, display_count, adc_count;
		uint16_t blink, blink_out, blink_alt;
		uint32_t Timer1, Timer2, Timer3, Timer4;
		int spi_flag, spi_flag0, spi_flag1;
	} V_data;

#ifdef	__cplusplus
}
#endif

#endif	/* MX_TEST_TYPES_H */

