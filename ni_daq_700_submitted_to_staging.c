/*
 *     comedi/drivers/ni_daq_700.c
 *     Driver for DAQCard-700 DIO/AI
 *     copied from 8255
 *
 *     COMEDI - Linux Control and Measurement Device Interface
 *     Copyright (C) 1998 David A. Schleef <ds@schleef.org>
 *
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/*
Driver: ni_daq_700
Description: National Instruments PCMCIA DAQCard-700 DIO/AI
Author: Fred Brooks <nsaspook@nsaspook.com>,
  based on ni_daq_dio24 by Daniel Vecino Castel <dvecino@able.es>
Devices: [National Instruments] PCMCIA DAQ-Card-700 (ni_daq_700)
Status: works
Updated: Wed, 19 Sep 2012 12:07:20 +0000

The daqcard-700 appears in Comedi as a  digital I/O subdevice (0) with
16 channels and a analog input subdevice (1) with 16 single-ended channels.

Digital:  The channel 0 corresponds to the daqcard-700's output
port, bit 0; channel 8 corresponds to the input port, bit 0.

Digital direction configuration: channels 0-7 output, 8-15 input (8225 device
emu as port A output, port B input, port C N/A).

Analog: The input  range is 0 to 4095 for -10 to +10 volts 
IRQ is assigned but not used.

Version 0.1	Original DIO only driver
Version 0.2	DIO and basic AI analog input support on 16 se channels

Manuals: 	Register level:	http://www.ni.com/pdf/manuals/340698.pdf
		User Manual: 	http://www.ni.com/pdf/manuals/320676d.pdf
*/

#include <linux/interrupt.h>
#include <linux/slab.h>
#include "../comedidev.h"

#include <linux/ioport.h>

#include <pcmcia/cistpl.h>
#include <pcmcia/cisreg.h>
#include <pcmcia/ds.h>

static struct pcmcia_device *pcmcia_cur_dev;

#define DAQ700_SIZE 0x0C		/*  size of io region used by board */

static int daq700_attach(struct comedi_device *dev,
			 struct comedi_devconfig *it);
static int daq700_detach(struct comedi_device *dev);

enum daq700_bustype { pcmcia_bustype };

struct daq700_board {
	const char *name;
	int device_id;		/*  device id for pcmcia board */
	enum daq700_bustype bustype;	/*  PCMCIA */
        int ai_chans;
        int ai_bits;
	/*  function pointers so we can use inb/outb or readb/writeb */
	/*  as appropriate */
	unsigned int (*read_byte) (unsigned int address);
	void (*write_byte) (unsigned int byte, unsigned int address);
};

static const struct daq700_board daq700_boards[] = {
	{
	 .name = "daqcard-700",
	  /*  0x10b is manufacturer id, 0x4743 is device id */
	 .device_id = 0x4743,
	 .bustype = pcmcia_bustype,
         .ai_chans = 16,
         .ai_bits = 12
	 },
	{
	 .name = "ni_daq_700",
	  /*  0x10b is manufacturer id, 0x4743 is device id */
	 .device_id = 0x4743,
	 .bustype = pcmcia_bustype,
         .ai_chans = 16,
         .ai_bits = 12
	 },
};

/*
 * Useful for shorthand access to the particular board structure
 */
#define thisboard ((const struct daq700_board *)dev->board_ptr)

struct daq700_private {

	int data;		/* number of data points left to be taken */
};

#define devpriv ((struct daq700_private *)dev->private)

static struct comedi_driver driver_daq700 = {
	.driver_name = "ni_daq_700",
	.module = THIS_MODULE,
	.attach = daq700_attach,
	.detach = daq700_detach,
	.num_names = ARRAY_SIZE(daq700_boards),
	.board_name = &daq700_boards[0].name,
	.offset = sizeof(struct daq700_board),
};

/*	the real driver routines	*/

#define _700_SIZE 0x0C
#define _700_DATA 0
/* daqcard700 registers */
#define DIO_W		0x04	// WO 8bit
#define DIO_R		0x05	// RO 8bit
#define CMD_R1		0x00	// WO 8bit
#define CMD_R2		0x07	// RW 8bit
#define CMD_R3		0x05	// W0 8bit
#define STA_R1		0x00	// RO 8bit
#define STA_R2		0x01	// RO 8bit
#define ADFIFO_R	0x02	// RO 16bit
#define ADCLEAR_R	0x01	// WO 8bit
#define CDA_R0		0x08	// RW 8bit
#define CDA_R1		0x09	// RW 8bit
#define CDA_R2		0x0A	// RW 8bit
#define CMO_R		0x0B	// RO 8bit
#define TIC_R		0x06	// WO 8bit

struct subdev_700_struct {
	unsigned long cb_arg;
	int (*cb_func) (int, int, int, unsigned long);
	int have_irq;
};

#define CALLBACK_ARG	(((struct subdev_700_struct *)s->private)->cb_arg)
#define CALLBACK_FUNC	(((struct subdev_700_struct *)s->private)->cb_func)
#define subdevpriv	((struct subdev_700_struct *)s->private)

static void subdev_700_ai_rinsn_config(struct comedi_device *dev, struct comedi_subdevice *s);
static int subdev_700_ai_rinsn(struct comedi_device *dev, struct comedi_subdevice *s,
                         struct comedi_insn *insn, unsigned int *data);


void subdev_700_interrupt(struct comedi_device *dev, struct comedi_subdevice *s)
{
	short d;

	d = CALLBACK_FUNC(0, _700_DATA, 0, CALLBACK_ARG);

	comedi_buf_put(s->async, d);
	s->async->events |= COMEDI_CB_EOS;

	comedi_event(dev, s);
}
EXPORT_SYMBOL(subdev_700_interrupt);

static int subdev_700_cb(int dir, int port, int data, unsigned long arg)
{
	/* port is always A for output and B for input (8255 emu) */
	unsigned long iobase = arg;

	if (dir) {
		outb(data, iobase + DIO_W);
		return 0;
	} else {
		return inb(iobase + DIO_R);
	}
}

static int subdev_700_dio_insn(struct comedi_device *dev,
			   struct comedi_subdevice *s, struct comedi_insn *insn,
			   unsigned int *data)
{
	if (data[0]) {
		s->state &= ~data[0];
		s->state |= (data[0] & data[1]);

		if (data[0] & 0xff)
			CALLBACK_FUNC(1, _700_DATA, s->state & 0xff,
				      CALLBACK_ARG);
	}

	data[1] = s->state & 0xff;
	data[1] |= CALLBACK_FUNC(0, _700_DATA, 0, CALLBACK_ARG) << 8;

	return 2;
}

static int subdev_700_dio_insn_config(struct comedi_device *dev,
				  struct comedi_subdevice *s,
				  struct comedi_insn *insn, unsigned int *data)
{

	switch (data[0]) {
	case INSN_CONFIG_DIO_INPUT:
		break;
	case INSN_CONFIG_DIO_OUTPUT:
		break;
	case INSN_CONFIG_DIO_QUERY:
		data[1] =
		    (s->
		     io_bits & (1 << CR_CHAN(insn->chanspec))) ? COMEDI_OUTPUT :
		    COMEDI_INPUT;
		return insn->n;
		break;
	default:
		return -EINVAL;
	}

	return 1;
}

static int subdev_700_ai_rinsn(struct comedi_device *dev, struct comedi_subdevice *s,
                         struct comedi_insn *insn, unsigned int *data)
{
        int n,i, chan;
        int d;
        unsigned int status;

        /* a typical programming sequence */
       	chan = ((insn->chanspec&0b00001111)|0b10000000);	// or mask scan bit high to disable scanning
        /* write channel to multiplexer */
        outb(chan,dev->iobase + CMD_R1);

        /* convert n samples */
        for (n = 0; n < insn->n; n++) {
                /* trigger conversion with out0 L to H */
		outb(0x00, dev->iobase + CMD_R2);	// enable ADC conversions
                outb(0b00110000,dev->iobase + CMO_R);	// mode 0 out0 L, from H
                outb(0b00110010,dev->iobase + CMO_R);	// mode 1 out0 H, L to H, start conversion
#define TIMEOUT 100
                /* wait for conversion to end */
                for (i = 0; i < TIMEOUT; i++) {
                        status = inb(dev->iobase + STA_R2);
                        if ((status&0b00000011)>0b00000000) {
                                printk(KERN_INFO "ni_daq_700: Overflow/run Error");
                                return -EOVERFLOW;
                        }
                        status = inb(dev->iobase + STA_R1);
                        if ((status&0b00000010)==0b00000010) {
                                printk(KERN_INFO "ni_daq_700: Data Error");
				return -ENODATA;
                        }
                        if ((status&0b00010001)==0b00000001) {	// ADC conversion complete
				break;
			}
                }
                if (i == TIMEOUT) {
        		printk(KERN_INFO "ni_daq_700: timeout during ADC conversion");
                        return -ETIMEDOUT;
                }

                /* read data */
                d = inw(dev->iobase + ADFIFO_R);
                /* mangle the data as necessary */

		// Bipolar Offset Binary: 0 to 4095 for -10 to +10
        	d &= 0x0fff;
        	d ^= 0x800;

                data[n] = d;
        }
		        /* return the number of samples read/written */ 
        return n;
}
static void subdev_700_ai_rinsn_config(struct comedi_device *dev, struct comedi_subdevice *s)
{			
/*
 Data acquisition is enabled.
 The counter 0 output is high.
 The I/O connector pin CLK1 drives counter 1 source.
 Multiple-channel scanning is disabled.
 All interrupts are disabled.
 The analog input range is set to +-10 V
 The analog input mode is single-ended.
 The analog input circuitry is initialized to channel 0.
 The A/D FIFO is cleared.
*/
	unsigned long iobase=dev->iobase;

	outb(0x80, iobase + CMD_R1);	// disable scanning, ADC to chan 0
        outb(0x00, iobase + CMD_R2);	// clear all bits
        outb(0x00, iobase + CMD_R3);	// set +-10 range
        outb(0x32, iobase + CMO_R);	// config counter mode1, out0 to H
        outb(0x00, iobase + TIC_R);	// clear counter interrupt
        outb(0x00, iobase + ADCLEAR_R);	// clear the ADC FIFO
	inw(iobase + ADFIFO_R);		// read 16bit junk from FIFO to clear all data
	return;
}

static int subdev_700_cmdtest(struct comedi_device *dev,
			      struct comedi_subdevice *s,
			      struct comedi_cmd *cmd)
{
	/* FIXME */

	return 0;
}

static int subdev_700_cmd(struct comedi_device *dev, struct comedi_subdevice *s)
{
	/* FIXME */

	return 0;
}

static int subdev_700_cancel(struct comedi_device *dev,
			     struct comedi_subdevice *s)
{
	/* FIXME */

	return 0;
}

int subdev_700_dio_init(struct comedi_device *dev, struct comedi_subdevice *s,
		    int (*cb) (int, int, int, unsigned long), unsigned long arg)
{
	s->type = COMEDI_SUBD_DIO;
	s->subdev_flags = SDF_READABLE | SDF_WRITABLE;
	s->n_chan = 16;
	s->range_table = &range_digital;
	s->maxdata = 1;

	s->private = kmalloc(sizeof(struct subdev_700_struct), GFP_KERNEL);
	if (!s->private)
		return -ENOMEM;

	CALLBACK_ARG = arg;
	if (cb == NULL)
		CALLBACK_FUNC = subdev_700_cb;
	 else
		CALLBACK_FUNC = cb;

	s->insn_bits = subdev_700_dio_insn;
	s->insn_config = subdev_700_dio_insn_config;

	s->state = 0;
	s->io_bits = 0x00ff;
	return 0;
}
EXPORT_SYMBOL(subdev_700_dio_init);

int subdev_700_ai_init(struct comedi_device *dev, struct comedi_subdevice *s,
                    int (*cb) (int, int, int, unsigned long), unsigned long arg)
{
        s->type = COMEDI_SUBD_AI;
        /* we support single-ended (ground)  */
        s->subdev_flags = SDF_READABLE | SDF_GROUND;
        s->n_chan = thisboard->ai_chans;
        s->maxdata = (1 << thisboard->ai_bits) - 1;
        s->range_table = &range_bipolar10;
        s->insn_read = subdev_700_ai_rinsn;
        s->private = kmalloc(sizeof(struct subdev_700_struct), GFP_KERNEL);
        if (!s->private)
                return -ENOMEM;
	subdev_700_ai_rinsn_config(dev,s);
        return 0;
}
EXPORT_SYMBOL(subdev_700_ai_init);

int subdev_700_init_irq(struct comedi_device *dev, struct comedi_subdevice *s,
			int (*cb) (int, int, int, unsigned long),
			unsigned long arg)
{
	int ret;

	ret = subdev_700_ai_init(dev, s, cb, arg);
	if (ret < 0)
		return ret;

	s->do_cmdtest = subdev_700_cmdtest;
	s->do_cmd = subdev_700_cmd;
	s->cancel = subdev_700_cancel;

	subdevpriv->have_irq = 1;

	return 0;
}
EXPORT_SYMBOL(subdev_700_init_irq);

void subdev_700_cleanup(struct comedi_device *dev, struct comedi_subdevice *s)
{
	if (s->private)
		if (subdevpriv->have_irq)

			kfree(s->private);
}
EXPORT_SYMBOL(subdev_700_cleanup);

static int daq700_attach(struct comedi_device *dev, struct comedi_devconfig *it)
{
	struct comedi_subdevice *s;
	unsigned long iobase = 0;
#ifdef incomplete
	unsigned int irq = 0;
#endif
	struct pcmcia_device *link;

	/* allocate and initialize dev->private */
	if (alloc_private(dev, sizeof(struct daq700_private)) < 0)
		return -ENOMEM;

	/*  get base address, irq etc. based on bustype */
	switch (thisboard->bustype) {
	case pcmcia_bustype:
		link = pcmcia_cur_dev;	/* XXX hack */
		if (!link)
			return -EIO;
		iobase = link->resource[0]->start;
#ifdef incomplete
		irq = link->irq;
#endif
		break;
	default:
		printk(KERN_ERR "bug! couldn't determine board type\n");
		return -EINVAL;
		break;
	}
	printk(KERN_ERR "comedi%d: ni_daq_700: %s, io 0x%lx", dev->minor,
	       thisboard->name, iobase);
#ifdef incomplete
	if (irq)
		printk(", irq %u", irq);

#endif

	printk("\n");

	if (iobase == 0) {
		printk(KERN_ERR "io base address is zero!\n");
		return -EINVAL;
	}

	dev->iobase = iobase;

#ifdef incomplete
	/* grab our IRQ */
	dev->irq = irq;
#endif

	dev->board_name = thisboard->name;

	if (alloc_subdevices(dev, 2) < 0)
		return -ENOMEM;

	/* DAQCard-700 dio */
	s = dev->subdevices + 0;
	subdev_700_dio_init(dev, s, NULL, dev->iobase);

        /* DAQCard-700 ai */
        s = dev->subdevices + 1;
        subdev_700_ai_init(dev, s, NULL, dev->iobase);
	return 0;
};

static int daq700_detach(struct comedi_device *dev)
{
	printk(KERN_ERR "comedi%d: ni_daq_700: cs-remove\n", dev->minor);

	if (dev->subdevices)
		subdev_700_cleanup(dev, dev->subdevices + 0);
        if (dev->subdevices)
                subdev_700_cleanup(dev, dev->subdevices + 1);

	if (thisboard->bustype != pcmcia_bustype && dev->iobase)
		release_region(dev->iobase, DAQ700_SIZE);
	if (dev->irq)
		free_irq(dev->irq, dev);

	return 0;
};

static void daq700_config(struct pcmcia_device *link);
static void daq700_release(struct pcmcia_device *link);
static int daq700_cs_suspend(struct pcmcia_device *p_dev);
static int daq700_cs_resume(struct pcmcia_device *p_dev);

static int daq700_cs_attach(struct pcmcia_device *);
static void daq700_cs_detach(struct pcmcia_device *);

struct local_info_t {
	struct pcmcia_device *link;
	int stop;
	struct bus_operations *bus;
};

static int daq700_cs_attach(struct pcmcia_device *link)
{
	struct local_info_t *local;

	printk(KERN_INFO "ni_daq_700:  cs-attach\n");

	dev_dbg(&link->dev, "daq700_cs_attach()\n");

	/* Allocate space for private device-specific data */
	local = kzalloc(sizeof(struct local_info_t), GFP_KERNEL);
	if (!local)
		return -ENOMEM;
	local->link = link;
	link->priv = local;

	pcmcia_cur_dev = link;

	daq700_config(link);

	return 0;
}				/* daq700_cs_attach */

static void daq700_cs_detach(struct pcmcia_device *link)
{

	printk(KERN_INFO "ni_daq_700: cs-detach!\n");

	dev_dbg(&link->dev, "daq700_cs_detach\n");

	((struct local_info_t *)link->priv)->stop = 1;
	daq700_release(link);

	/* This points to the parent struct local_info_t struct */
	kfree(link->priv);

}				/* daq700_cs_detach */

static int daq700_pcmcia_config_loop(struct pcmcia_device *p_dev,
				void *priv_data)
{
	if (p_dev->config_index == 0)
		return -EINVAL;

	return pcmcia_request_io(p_dev);
}

static void daq700_config(struct pcmcia_device *link)
{
	int ret;

	printk(KERN_INFO "ni_daq_700:  cs-config\n");

	dev_dbg(&link->dev, "daq700_config\n");

	link->config_flags |= CONF_ENABLE_IRQ | CONF_AUTO_AUDIO |
		CONF_AUTO_SET_IO;

	ret = pcmcia_loop_config(link, daq700_pcmcia_config_loop, NULL);
	if (ret) {
		dev_warn(&link->dev, "no configuration found\n");
		goto failed;
	}

	if (!link->irq)
		goto failed;

	ret = pcmcia_enable_device(link);
	if (ret != 0)
		goto failed;

	return;

failed:
	printk(KERN_INFO "ni_daq_700 cs failed");
	daq700_release(link);

}				/* daq700_config */

static void daq700_release(struct pcmcia_device *link)
{
	dev_dbg(&link->dev, "daq700_release\n");

	pcmcia_disable_device(link);
}				/* daq700_release */

static int daq700_cs_suspend(struct pcmcia_device *link)
{
	struct local_info_t *local = link->priv;

	/* Mark the device as stopped, to block IO until later */
	local->stop = 1;
	return 0;
}				/* daq700_cs_suspend */

static int daq700_cs_resume(struct pcmcia_device *link)
{
	struct local_info_t *local = link->priv;

	local->stop = 0;
	return 0;
}				/* daq700_cs_resume */

/*====================================================================*/

static const struct pcmcia_device_id daq700_cs_ids[] = {
	/* N.B. These IDs should match those in daq700_boards */
	PCMCIA_DEVICE_MANF_CARD(0x010b, 0x4743),	/* daqcard-700 */
	PCMCIA_DEVICE_NULL
};


MODULE_DEVICE_TABLE(pcmcia, daq700_cs_ids);
MODULE_AUTHOR("Fred Brooks <nsaspook@nsaspook.com>");
MODULE_DESCRIPTION("Comedi driver for National Instruments "
		   "PCMCIA DAQCard-700 DIO/AI ");
MODULE_VERSION("0.2.00");
MODULE_LICENSE("GPL");

struct pcmcia_driver daq700_cs_driver = {
	.probe = daq700_cs_attach,
	.remove = daq700_cs_detach,
	.suspend = daq700_cs_suspend,
	.resume = daq700_cs_resume,
	.id_table = daq700_cs_ids,
	.owner = THIS_MODULE,
	.name = "ni_daq_700",
};

static int __init init_daq700_cs(void)
{
	pcmcia_register_driver(&daq700_cs_driver);
	return 0;
}

static void __exit exit_daq700_cs(void)
{
	pr_debug("ni_daq_700: unloading\n");
	pcmcia_unregister_driver(&daq700_cs_driver);
}

int __init init_module(void)
{
	int ret;

	ret = init_daq700_cs();
	if (ret < 0)
		return ret;

	return comedi_driver_register(&driver_daq700);
}

void __exit cleanup_module(void)
{
	exit_daq700_cs();
	comedi_driver_unregister(&driver_daq700);
}
