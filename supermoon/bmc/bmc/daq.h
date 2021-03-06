/* 
 * File:   daq.h
 * Author: root
 *
 * Created on September 21, 2012, 6:49 PM
 */

/*
 Current sensor pins to ribbon cable:
 *      wire 1  pin 3 output
 *      wire 3  pin 2 ground
 *      wire 5  pin 1 power VS 5vdc
 *      wire 9  pv voltage from cal pot
 *      wire 10 PV ground
 */


#ifndef DAQ_H
#define	DAQ_H

#ifdef	__cplusplus
extern "C" {
#endif

#define PVV_C       0
#define PVV_NULL    4
#define PVV_NULL_TIME   200
#define PVV_NULL_TIME_RAW   10


#define LPCHANC        16

#define RANGE_2_048   0
#define RANGE_1_024   1
#define RANGE_0_512   2
#define ADGAIN1		1.0002
#define ADGAIN2		1.0003   

#include <comedilib.h>
#include "bmc.h"

    int subdev_ai; /* change this to your input subdevice */
    int chan_ai; /* change this to your channel */
    int range_ai; /* more on this later */
    int aref_ai; /* more on this later */
    int maxdata_ai, ranges_ai, channels_ai;

    int subdev_dio; /* change this to your input subdevice */
    int chan_dio; /* change this to your channel */
    int range_dio; /* more on this later */
    int aref_dio; /* more on this later */
    int maxdata_dio, ranges_dio, channels_dio, datain_dio;

    comedi_t *it;
    comedi_range *ad_range;
    int8_t ADC_OPEN, DIO_OPEN, ADC_ERROR, DEV_OPEN, DIO_ERROR;

    extern struct bmcdata bmc;
    extern struct didata datain;
    extern struct dodata dataout;
    extern unsigned char HAVE_DIO, HAVE_AI;

    int init_daq(void);
    int init_dio(void);
    int adc_range(double, double);
    double get_adc_volts(int, int, int);
    int get_dio_bit(int);
    int put_dio_bit(int, int);
    int get_data_sample(void);
    double lp_filter(double, int, int), gain_adj;
#ifdef	__cplusplus
}
#endif

#endif	/* DAQ_H */

