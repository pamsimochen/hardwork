/*
 * Copyright (C) 2009, Texas Instruments, Incorporated
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <asm/arch/nand.h>
#include <linux/mtd/nand.h>
#include <miiphy.h>
#include <nand.h>
#include <spi.h>
#include <asm/arch/hardware.h>
#include <asm/io.h>
#include <mmc.h>
#include <i2c.h>
#include <asm/arch/sys_proto.h>


typedef int		Bool;
#define TRUE	((Bool) 1)
#define FALSE	((Bool) 0)


extern void ipnc_ff_pll_init(int);

static int do_cmd_ipnc_ff_init(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int val;
	char ch;
	
	if (argc != 2) {
		cmd_usage(cmdtp);
		return 1;
	}
	
	argc--;
	argv++;
	
	ch = *argv[0];
	if(ch == '1')
		val = 1;
	else
		val = 0;
	
	ipnc_ff_pll_init(val);
	
	return 0;

}

U_BOOT_CMD(
	ipnc_ff_init, 2, 1, do_cmd_ipnc_ff_init,
	"IPNC Full feature mode script",
	"ipnc_ff_init 0:1	[DSS & DSP will be OFF:ON]\n"
);
