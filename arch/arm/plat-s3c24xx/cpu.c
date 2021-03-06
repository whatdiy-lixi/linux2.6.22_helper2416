/* linux/arch/arm/plat-s3c24xx/cpu.c
 *
 * Copyright (c) 2004-2005 Simtec Electronics
 *	http://www.simtec.co.uk/products/SWLINUX/
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * S3C24XX CPU Support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/serial_core.h>
#include <linux/platform_device.h>

#include <asm/hardware.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/delay.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>

#include <asm/arch/regs-gpio.h>
#include <asm/arch/regs-serial.h>

#include <asm/plat-s3c24xx/cpu.h>
#include <asm/plat-s3c24xx/devs.h>
#include <asm/plat-s3c24xx/clock.h>
#include <asm/plat-s3c24xx/s3c2400.h>
#include <asm/plat-s3c24xx/s3c2410.h>
#include <asm/plat-s3c24xx/s3c2412.h>
#include "s3c244x.h"
#include <asm/plat-s3c24xx/s3c2440.h>
#include <asm/plat-s3c24xx/s3c2442.h>
#include <asm/plat-s3c24xx/s3c2443.h>
/* 新增加 */
#include <asm/plat-s3c24xx/s3c2416.h>
/* 新增加 结束*/
struct cpu_table {
	unsigned long	idcode;
	unsigned long	idmask;
	void		(*map_io)(struct map_desc *mach_desc, int size);
	void		(*init_uarts)(struct s3c2410_uartcfg *cfg, int no);
	void		(*init_clocks)(int xtal);
	int		(*init)(void);
	const char	*name;
};

/* table of supported CPUs */

static const char name_s3c2400[]  = "S3C2400";
static const char name_s3c2410[]  = "S3C2410";
static const char name_s3c2412[]  = "S3C2412";
static const char name_s3c2440[]  = "S3C2440";
static const char name_s3c2442[]  = "S3C2442";
static const char name_s3c2443[]  = "S3C2443";
static const char name_s3c2410a[] = "S3C2410A";
static const char name_s3c2440a[] = "S3C2440A";
static const char name_s3c2416b[]  = "S3C2416 EVT3";

static struct cpu_table cpu_ids[] __initdata = {
	{
		.idcode		= 0x32410000,
		.idmask		= 0xffffffff,
		.map_io		= s3c2410_map_io,
		.init_clocks	= s3c2410_init_clocks,
		.init_uarts	= s3c2410_init_uarts,
		.init		= s3c2410_init,
		.name		= name_s3c2410
	},
	{
		.idcode		= 0x32410002,
		.idmask		= 0xffffffff,
		.map_io		= s3c2410_map_io,
		.init_clocks	= s3c2410_init_clocks,
		.init_uarts	= s3c2410_init_uarts,
		.init		= s3c2410_init,
		.name		= name_s3c2410a
	},
	{
		.idcode		= 0x32440000,
		.idmask		= 0xffffffff,
		.map_io		= s3c244x_map_io,
		.init_clocks	= s3c244x_init_clocks,
		.init_uarts	= s3c244x_init_uarts,
		.init		= s3c2440_init,
		.name		= name_s3c2440
	},
	{
		.idcode		= 0x32440001,
		.idmask		= 0xffffffff,
		.map_io		= s3c244x_map_io,
		.init_clocks	= s3c244x_init_clocks,
		.init_uarts	= s3c244x_init_uarts,
		.init		= s3c2440_init,
		.name		= name_s3c2440a
	},
	{
		.idcode		= 0x32440aaa,
		.idmask		= 0xffffffff,
		.map_io		= s3c244x_map_io,
		.init_clocks	= s3c244x_init_clocks,
		.init_uarts	= s3c244x_init_uarts,
		.init		= s3c2442_init,
		.name		= name_s3c2442
	},
	{
		.idcode		= 0x32412001,
		.idmask		= 0xffffffff,
		.map_io		= s3c2412_map_io,
		.init_clocks	= s3c2412_init_clocks,
		.init_uarts	= s3c2412_init_uarts,
		.init		= s3c2412_init,
		.name		= name_s3c2412,
	},
	{			/* a newer version of the s3c2412 */
		.idcode		= 0x32412003,
		.idmask		= 0xffffffff,
		.map_io		= s3c2412_map_io,
		.init_clocks	= s3c2412_init_clocks,
		.init_uarts	= s3c2412_init_uarts,
		.init		= s3c2412_init,
		.name		= name_s3c2412,
	},
	{
		.idcode		= 0x32443001,
		.idmask		= 0xffffffff,
		.map_io		= s3c2443_map_io,
		.init_clocks	= s3c2443_init_clocks,
		.init_uarts	= s3c2443_init_uarts,
		.init		= s3c2443_init,
		.name		= name_s3c2443,
	},
	{
		.idcode		= 0x0,   /* S3C2400 doesn't have an idcode */
		.idmask		= 0xffffffff,
		.map_io		= s3c2400_map_io,
		.init_clocks	= s3c2400_init_clocks,
		.init_uarts	= s3c2400_init_uarts,
		.init		= s3c2400_init,
		.name		= name_s3c2400
	},
#if 0
	/* 新增加的s3c2416的条目 */
	{			/* Evt3 s3c2416 */
		.idcode		= 0x32450003,
		.idmask		= 0xffffffff,
		.map_io		= s3c2416_map_io,
		.init_clocks	= s3c2416_init_clocks,
		.init_uarts	= s3c2416_init_uarts,
		.init		= s3c2416_init,
		.name		= name_s3c2416b,
	},
	/* 新增加的s3c2416的条目 */
	{			/* Evt3 s3c2416 */
		.idcode		= 0x32450003,
		.idmask		= 0xffffffff,
		.map_io		= s3c244x_map_io,
		.init_clocks	= s3c244x_init_clocks,
		.init_uarts	= s3c244x_init_uarts,
		.init		= s3c2440_init,
		.name		= name_s3c2416b,
	},
	/* 新增加的s3c2416的条目 */
	{			/* Evt3 s3c2416 */
		.idcode		= 0x32450003,
		.idmask		= 0xffffffff,
		.map_io		= s3c244x_map_io,
		.init_clocks	= s3c2416_init_clocks,
		.init_uarts	= s3c244x_init_uarts,
		.init		= s3c2440_init,
		.name		= name_s3c2416b,
	},
	/* 新增加的s3c2416的条目 */
	{			/* Evt3 s3c2416 */
		.idcode		= 0x32450003,
		.idmask		= 0xffffffff,
		.map_io		= s3c2416_map_io,
		.init_clocks	= s3c2416_init_clocks,
		.init_uarts	= s3c244x_init_uarts,
		.init		= s3c2440_init,
		.name		= name_s3c2416b,
	},
	#endif
	/* 新增加的s3c2416的条目 */
	{			/* Evt3 s3c2416 */
		.idcode		= 0x32450003,
		.idmask		= 0xffffffff,
		.map_io		= s3c2416_map_io,
		.init_clocks	= s3c2416_init_clocks,
		.init_uarts	= s3c244x_init_uarts,
		.init		= s3c2416_init,
		.name		= name_s3c2416b,
	},
};

/* minimal IO mapping */

static struct map_desc s3c_iodesc[] __initdata = {
	IODESC_ENT(GPIO),	/* S3C24XX_VA_GPIO */
	IODESC_ENT(IRQ),
	IODESC_ENT(MEMCTRL),
	IODESC_ENT(UART),

	/* 新增加的 */
	IODESC_ENT(SROMC),
	IODESC_ENT(DMC0),
	IODESC_ENT(DMC1),
	IODESC_ENT(SYSCON)
	/* 新增加 结束 */
};


static struct cpu_table *
s3c_lookup_cpu(unsigned long idcode)
{
	struct cpu_table *tab;
	int count;

	tab = cpu_ids;
	for (count = 0; count < ARRAY_SIZE(cpu_ids); count++, tab++) {
		if ((idcode & tab->idmask) == tab->idcode)
			return tab;
	}

	return NULL;
}

/* 新增加 */
/* board information */

static struct s3c24xx_board *board;

void s3c24xx_set_board(struct s3c24xx_board *b)
{
	int i;

	board = b;

	if (b->clocks_count != 0) {
		struct clk **ptr = b->clocks;

		for (i = b->clocks_count; i > 0; i--, ptr++)
			if (s3c24xx_register_clock(*ptr) < 0)
				printk(KERN_ERR "failed to register clock.\n");
	}
}

/* 新增加 结束 */

/* cpu information */

static struct cpu_table *cpu;

static unsigned long s3c24xx_read_idcode_v5(void)
{
#if 0
#if defined(CONFIG_CPU_S3C2412) || defined(CONFIG_CPU_S3C2413)
	return __raw_readl(S3C2412_GSTATUS1);
#else
	return 1UL;	/* don't look like an 2400 */
#endif
#endif

/* 新增加的 */
#if defined(CONFIG_CPU_S3C2412) || defined(CONFIG_CPU_S3C2413)
	return __raw_readl(S3C2412_GSTATUS1);
#elif defined(CONFIG_CPU_S3C2450) || defined(CONFIG_CPU_S3C2416)
	return __raw_readl(S3C2410_GSTATUS1);
#else
	return 1UL;	/* don't look like an 2400 */
#endif
/* 新增加 结束 */
}

static unsigned long s3c24xx_read_idcode_v4(void)
{
#ifndef CONFIG_CPU_S3C2400
	return __raw_readl(S3C2410_GSTATUS1);
#else
	return 0UL;
#endif
}

void __init s3c24xx_init_io(struct map_desc *mach_desc, int size)
{
	/********* add here *********/
	putstring_VA("s3c24xx_init_io()\r\n");

//	extern unsigned int processor_id;
//	processor_id = 0x41129200;
//	processor_id = 0x41069265;
	/********* end add *********/
	unsigned long idcode = 0x0;

	/* initialise the io descriptors we need for initialisation */
	iotable_init(s3c_iodesc, ARRAY_SIZE(s3c_iodesc));

	if (cpu_architecture() >= CPU_ARCH_ARMv5) {
		idcode = s3c24xx_read_idcode_v5();	/* 进入这里 */
	} else {
		idcode = s3c24xx_read_idcode_v4();
	}

	/********* add here *********/
	putstring_VA("idcode=");	/* idcode=0x32450003 */
	putinthex_VA(idcode);
	/********* end add *********/

	cpu = s3c_lookup_cpu(idcode);

	/********* add here *********/
	putstring_VA("cpu=");
	putinthex_VA(cpu);
	/********* end add *********/

	/* 读取到的idcode=chipid=0x32450003，s3c_lookup_cpu找不到对应的ID，在cpu_ids中增加对应条目 */
	
	if (cpu == NULL) {
		printk(KERN_ERR "Unknown CPU type 0x%08lx\n", idcode);
		panic("Unknown S3C24XX CPU");
	}

	printk("CPU %s (id 0x%08lx)\n", cpu->name, idcode);

	if (cpu->map_io == NULL || cpu->init == NULL) {
		printk(KERN_ERR "CPU %s support not enabled\n", cpu->name);
		panic("Unsupported S3C24XX CPU");
	}

	(cpu->map_io)(mach_desc, size);

	/********* add here *********/
	putstring_VA("end of s3c24xx_init_io()\r\n");
	/********* end add *********/
}

/* s3c24xx_init_clocks
 *
 * Initialise the clock subsystem and associated information from the
 * given master crystal value.
 *
 * xtal  = 0 -> use default PLL crystal value (normally 12MHz)
 *      != 0 -> PLL crystal value in Hz
*/

void __init s3c24xx_init_clocks(int xtal)
{
	/********* add here *********/
	putstring_VA("s3c24xx_init_clocks()\r\n");
	/********* end add *********/
	if (xtal == 0)
		xtal = 12*1000*1000;

	if (cpu == NULL)
		panic("s3c24xx_init_clocks: no cpu setup?\n");

	if (cpu->init_clocks == NULL)
		panic("s3c24xx_init_clocks: cpu has no clock init\n");
	else
		(cpu->init_clocks)(xtal);

	/********* add here *********/
	putstring_VA("end of s3c24xx_init_clocks()\r\n");
	/********* end add *********/
}

/* uart management */

static int nr_uarts __initdata = 0;

static struct s3c2410_uartcfg uart_cfgs[3];

/* s3c24xx_init_uartdevs
 *
 * copy the specified platform data and configuration into our central
 * set of devices, before the data is thrown away after the init process.
 *
 * This also fills in the array passed to the serial driver for the
 * early initialisation of the console.
*/

void __init s3c24xx_init_uartdevs(char *name,
				  struct s3c24xx_uart_resources *res,
				  struct s3c2410_uartcfg *cfg, int no)
{
	struct platform_device *platdev;
	struct s3c2410_uartcfg *cfgptr = uart_cfgs;
	struct s3c24xx_uart_resources *resp;
	int uart;

	memcpy(cfgptr, cfg, sizeof(struct s3c2410_uartcfg) * no);

	for (uart = 0; uart < no; uart++, cfg++, cfgptr++) {
		platdev = s3c24xx_uart_src[cfgptr->hwport];

		resp = res + cfgptr->hwport;

		s3c24xx_uart_devs[uart] = platdev;

		platdev->name = name;
		platdev->resource = resp->resources;
		platdev->num_resources = resp->nr_resources;

		platdev->dev.platform_data = cfgptr;
	}

	nr_uarts = no;
}

void __init s3c24xx_init_uarts(struct s3c2410_uartcfg *cfg, int no)
{
	/********* add here *********/
	putstring_VA("s3c24xx_init_uarts()\r\n");
	/********* end add *********/
	if (cpu == NULL)
		return;

	if (cpu->init_uarts == NULL) {
		printk(KERN_ERR "s3c24xx_init_uarts: cpu has no uart init\n");
	} else
		(cpu->init_uarts)(cfg, no);

	/********* add here *********/
	putstring_VA("end of s3c24xx_init_uarts()\r\n");
	/********* end add *********/
}

static int __init s3c_arch_init(void)
{
	int ret;

	// do the correct init for cpu

	if (cpu == NULL)
		panic("s3c_arch_init: NULL cpu\n");

	ret = (cpu->init)();	/* s3c2416_init()。cpu在s3c24xx_init_io()中被赋值,指向cpu_ids[]中的一个成员 */
	if (ret != 0)
		return ret;

	ret = platform_add_devices(s3c24xx_uart_devs, nr_uarts);

#if 0
	/* 可以在smdk2416_machine_init()中注册设备 */
	/* 这里需要添加platform_device_register() */
	if (ret != 0)
		return ret;

	if (board != NULL) {
		struct platform_device **ptr = board->devices;
		int i;

		for (i = 0; i < board->devices_count; i++, ptr++) {
			ret = platform_device_register(*ptr);

			if (ret) {
				printk(KERN_ERR "s3c24xx: failed to add board device %s (%d) @%p\n", (*ptr)->name, ret, *ptr);
			}
		}

		/* mask any error, we may not need all these board
		 * devices */
		ret = 0;
	}
#endif

	return ret;
}

arch_initcall(s3c_arch_init);
