/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* for now: just dummy functions to satisfy the linker */

#include <common.h>

void __flush_cache (unsigned long dummy1, unsigned long dummy2)
{
#ifdef CONFIG_OMAP2420
	void arm1136_cache_flush(void);

	arm1136_cache_flush();
#endif
	return;
}
void flush_cache (unsigned long dummy1, unsigned long dummy2)
	 __attribute__((weak, alias("__flush_cache")));

/*
 * Default implementation of enable_caches()
 * Real implementation should be in platform code
 */
void __enable_caches(void)
{
	puts("Off\n");
}
void enable_caches(void)
	__attribute__((weak, alias("__enable_caches")));
