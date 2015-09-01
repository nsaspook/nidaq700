/*
 * Tutorial example #1
 * Part of Comedilib
 *
 * Copyright (c) 1999,2000 David A. Schleef <ds@schleef.org>
 *
 * This file may be freely modified, distributed, and combined with
 * other software, as long as proper attribution is given in the
 * source code.
 */

#include <stdio.h>	/* for printf() */
#include <unistd.h>
#include <comedilib.h>

int subdev = 0;		/* change this to your input subdevice */
int chan = 0;		/* change this to your channel */
int range = 0;		/* more on this later */
int aref = AREF_GROUND;	/* more on this later */

int main(int argc,char *argv[])
{
	comedi_t *it;
	int i=0;
	lsampl_t data;
	int retval,maxdata,ranges,channels;
	double	volts;
	comedi_range *ad_range;

	it = comedi_open("/dev/comedi0");
	if(it == NULL) {
		comedi_perror("comedi_open");
		return -1;
	}
	subdev=comedi_find_subdevice_by_type(it,COMEDI_SUBD_AI,subdev);
	if (subdev<0) return subdev;

	channels=comedi_get_n_channels(it,subdev);
        printf("Channels %i ",channels);
	maxdata=comedi_get_maxdata(it,subdev,i);
        printf("Maxdata %i ",maxdata);
        ranges=comedi_get_n_ranges(it,subdev,i);
        printf("Ranges %i ",ranges);
	ad_range=comedi_get_range (it, subdev, i,ranges-1);
  	printf (": ad_range .min = %.1f, max = %.1f\n", ad_range->min,
               ad_range->max);
	while (1) {
        	printf("         \r");
		for (i=0;i<channels;i++) {
			retval = comedi_data_read_delayed(it, subdev, i, range, aref, &data,10000);
			if(retval < 0)
			{
			comedi_perror("comedi_data_read");
			return -1;
			}
			volts=comedi_to_phys(data, ad_range, maxdata);

                        printf(" %4i %.1fV", data,volts);
		}
//		usleep(49999);
	}
	return 0;
}

