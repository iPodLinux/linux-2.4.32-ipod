/****************************************************************************/

/*
 *      snaparm.c -- mappings for SnapGear ARM based boards
 *
 *      (C) Copyright 2000-2005, Greg Ungerer (gerg@snapgear.com)
 *      (C) Copyright 2001-2005, SnapGear (www.snapgear.com)
 *
 *	I expect most SnapGear ARM based boards will have similar
 *	flash arrangements. So this map driver can handle them all.
 */

/****************************************************************************/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/blk.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/cfi.h>
#include <linux/reboot.h>
#include <linux/ioport.h>
#include <asm/io.h>

/****************************************************************************/

static struct mtd_info *sg_mtd;
static struct resource *sg_res;

/****************************************************************************/

/*
 *	Define physical addresss flash is mapped at. Will be different on
 *	different boards.
 */
#if defined(CONFIG_ARCH_SE4000) || defined(CONFIG_MACH_IVPN) || \
    defined(CONFIG_MACH_SG560) || defined(CONFIG_MACH_SG580)
#define	FLASH_ADDR	0x50000000		/* Physical flash address */
#define	FLASH_SIZE	0x01000000		/* Maximum flash size */
#define	FLASH_WIDTH	2			/* Flash bus width */

#if defined(CONFIG_MACH_SG560)
#define CONFIG_SIZE	0x80000
#elif defined(CONFIG_MACH_SG580)
#define CONFIG_SIZE	0x100000
#else
#define CONFIG_SIZE	0x20000
#endif

#endif

#if defined(CONFIG_MACH_SG565) || defined(CONFIG_MACH_SHIVA1100)
#define	FLASH_ADDR	0x50000000		/* Physical flash address */
#define	FLASH_SIZE	0x01000000		/* Maximum flash size */
#define	FLASH_WIDTH	1			/* Flash bus width */

#if defined(CONFIG_MACH_SG565)
#define CONFIG_SIZE	0x100000
#else
#define CONFIG_SIZE	0x20000
#endif

#endif

#if defined(CONFIG_MACH_LITE300) || defined(CONFIG_MACH_SE4200)
#define	FLASH_ADDR	0x02000000		/* Physical flash address */
#define	FLASH_SIZE	0x00800000		/* Maximum flash size */
#define	FLASH_WIDTH	1			/* Flash bus width */
#define CONFIG_SIZE	0x20000
#endif

#if defined(CONFIG_MACH_CM4008)
#define	FLASH_ADDR	0x02000000		/* Physical flash address */
#define	FLASH_SIZE	0x00800000		/* Maximum flash size */
#define	FLASH_WIDTH	1			/* Flash bus width */
#endif

#if defined(CONFIG_MACH_CM41xx)
#define	FLASH_ADDR	0x02000000		/* Physical flash address */
#define	FLASH_SIZE	0x01000000		/* Maximum flash size */
#define	FLASH_WIDTH	1			/* Flash bus width */
#endif

#if defined(CONFIG_MACH_EP9312)
#define	FLASH_ADDR	0x60000000		/* Physical flash address */
#define	FLASH_SIZE	0x00800000		/* Maximum flash size */
#define	FLASH_WIDTH	2			/* Flash bus width */
#define CONFIG_SIZE	0x20000
#endif

#if defined(CONFIG_MACH_IPD)
#define	FLASH_ADDR	0x00000000		/* Physical flash address */
#define	FLASH_SIZE	0x01000000		/* Maximum flash size */
#define	FLASH_WIDTH	2			/* Flash bus width */
#endif

#if defined(CONFIG_MACH_SE5100)
#define	FLASH_ADDR	0x50000000		/* Physical flash address */
#define	FLASH_SIZE	0x02000000		/* Maximum flash size */
#define	FLASH_WIDTH	2			/* Flash bus width */

#define	BOOT_OFFSET	0x00000000
#define	BOOT_SIZE	0x00040000

#define	RECOVER_OFFSET	0x00040000
#define	RECOVER_SIZE	0x00800000

#define	KERNEL_OFFSET	(BOOT_SIZE + RECOVER_SIZE)
#define	KERNEL_SIZE	0x00180000
#define CONFIG_SIZE	0x00020000
#define	NG_CONFIG_SIZE	0x00200000
#define	NG_VAR_SIZE	0x00200000

#define	ROOTFS_SIZE	(FLASH_SIZE - BOOT_SIZE - KERNEL_SIZE - CONFIG_SIZE - \
			 NG_CONFIG_SIZE - NG_VAR_SIZE - RECOVER_SIZE)

#endif

/****************************************************************************/

//#define SNAPARM_DEBUG 1
#if SNAPARM_DEBUG
#define DPRINTK printk
#else
#define DPRINTK(...)
#endif

/****************************************************************************/

/*
 *	Define some access helper macros. On different architectures
 *	we have to deal with multi-byte quanitites, read/write buffers,
 *	and other architectural details a little differently. These
 *	macros try to abstract that as much as possible to keep the
 *	code clean.
 */

#if CONFIG_ARCH_KS8695
/*
 *	The bus read and write buffers can potenitially coalesce read and
 *	write bus cycles to the same address, thus dropping real cycles
 *	when talking to IO type devices. We need to flush those	buffers 
 *	when doing flash reading/writing.
 *
 *	Walk through a small section of memory avoiding the cache so that we
 *	can keep the flash running smoothly. Using the write buffer enable
 *	disable seems tocause nasty bus junk, sodon't use them.
 */
static void invalidate_buffer(void)
{
	static int z = 0;
	*((volatile unsigned int *) (0xc0010000 + (z++ & 0xffff)));
}
#define	readpreflush(x)		invalidate_buffer()
#define	writepreflush(x)	invalidate_buffer()

#define	CONFIG_LOCK_MULTIBYTE
#define	initlock()		unsigned long flags
#define	getlock()		save_flags(flags); cli()
#define	releaselock()		restore_flags(flags)
#endif	/* CONFIG_ARCH_KS8695 */


/*
 *	We are not entirely sure why, but on the iVPN the timing _between_
 *	access to the flash causes problems with other bus activity on the
 *	expansion bus. Namely the CompactFlash WiFi card. Delaying 1us
 *	is enough to clean up the cycles.
 */
#if CONFIG_MACH_IVPN
#define	readpreflush(x)		udelay(1)
#define	readpostflush(x)	udelay(1)
#define	writepreflush(x)	udelay(1)
#define	writepostflush(x)	udelay(1)

#define	CONFIG_LOCK_MULTIBYTE
#define	initlock()		unsigned long flags
#define	getlock()		save_flags(flags); cli()
#define	releaselock()		restore_flags(flags)
#endif	/* CONFIG_MACH_IVPN */


/*
 *	Now default any macros that are not used.
 */
#ifndef readpreflush
#define	readpreflush(x)
#endif
#ifndef readpostflush
#define	readpostflush(x)
#endif
#ifndef writepreflush
#define	writepreflush(x)
#endif
#ifndef writepostflush
#define	writepostflush(x)
#endif
#ifndef initlock
#define	initlock()
#endif
#ifndef getlock
#define	getlock()
#endif
#ifndef releaselock
#define	releaselock()
#endif

/****************************************************************************/

static __u8 sg_read8(struct map_info *map, unsigned long ofs)
{
	__u8 res;
	readpreflush(map->map_priv_1 + ofs);
	res = readb(map->map_priv_1 + ofs);
	readpostflush(map->map_priv_1 + ofs);
	DPRINTK("%s(0x%x) = 0x%x\n", __FUNCTION__, (__u32)ofs, (__u32)res);
	return(res);
}

static __u16 sg_read16(struct map_info *map, unsigned long ofs)
{
	__u16 res;
	readpreflush(map->map_priv_1 + ofs);
	res = readw(map->map_priv_1 + ofs);
	readpostflush(map->map_priv_1 + ofs);
	DPRINTK("%s(0x%x) = 0x%x\n", __FUNCTION__, (__u32)ofs, (__u32)res);
	return(res);
}

static __u32 sg_read32(struct map_info *map, unsigned long ofs)
{
	__u32 res;
	initlock();

	readpreflush(map->map_priv_1 + ofs);
	getlock();
	res = readl(map->map_priv_1 + ofs);
	releaselock();
	readpostflush(map->map_priv_1 + ofs);
	DPRINTK("%s(0x%x) = 0x%x\n", __FUNCTION__, (__u32)ofs, (__u32)res);
	return(res);
}

static void sg_copy_from(struct map_info *map, void *to, unsigned long from, ssize_t len)
{
	DPRINTK("%s(to=0x%x, from=0x%x, len=%d)\n", __FUNCTION__,
		(__u32)to, (__u32)from, (__u32)len);

#ifdef CONFIG_LOCK_MULTIBYTE
{
	unsigned short *dst;
	unsigned int i;
	initlock();

	getlock();
	for (dst = to, i = 0; (i < len); i += 2) {
		readpreflush(map->map_priv_1 + from + i);
		*dst++ = readw(map->map_priv_1 + from + i);
		readpostflush(map->map_priv_1 + from + i);
	}
	releaselock();
}
#else
	memcpy_fromio(to, map->map_priv_1 + from, len);
#endif
}

static void sg_write8(struct map_info *map, __u8 d, unsigned long adr)
{
	DPRINTK("%s(0x%x,0x%x)\n", __FUNCTION__, (__u32)d, (__u32)adr);
	writepreflush(map->map_priv_1 + adr);
	writeb(d, map->map_priv_1 + adr);
	writepostflush(map->map_priv_1 + adr);
}

static void sg_write16(struct map_info *map, __u16 d, unsigned long adr)
{
	DPRINTK("%s(0x%x,0x%x)\n", __FUNCTION__, (__u32)d, (__u32)adr);
	writepreflush(map->map_priv_1 + adr);
	writew(d, map->map_priv_1 + adr);
	writepostflush(map->map_priv_1 + adr);
}

static void sg_write32(struct map_info *map, __u32 d, unsigned long adr)
{
	initlock();

	DPRINTK("%s(0x%x,0x%x)\n", __FUNCTION__, (__u32)d, (__u32)adr);
	writepreflush(map->map_priv_1 + adr);
	getlock();
	writel(d, map->map_priv_1 + adr);
	releaselock();
	writepostflush(map->map_priv_1 + adr);
}

static void sg_copy_to(struct map_info *map, unsigned long to, const void *from, ssize_t len)
{
	DPRINTK("%s(to=0x%x,from=0x%x,len=%d)\n", __FUNCTION__,
		(__u32)to, (__u32)from, (__u32)len);

#ifdef CONFIG_LOCK_MULTIBYTE
{
	unsigned short *src;
	unsigned int i;
	initlock();

	getlock();
	for (src = from, i = 0; (i < len); i += 2) {
		writepreflush(map->map_priv_1 + from + i);
		writew(*src++, map->map_priv_1 + from + i);
		writepostflush(map->map_priv_1 + from + i);
	}
	releaselock();
}
#else
	memcpy_toio(map->map_priv_1 + to, from, len);
#endif
}

/****************************************************************************/
/*                           OPENGEAR FLASH                                 */
/****************************************************************************/
#if defined(CONFIG_MACH_CM4008) || defined(CONFIG_MACH_CM41xx)

#define	VENDOR		"OpenGear"
#define	VENDOR_ROOTFS	"OpenGear image"

/*
 *	Intel FLASH setup. This is the only flash device, it is the entire
 *	non-volatile storage (no IDE CF or hard drive or anything else).
 */
static struct map_info sg_map = {
	name: "OpenGear Intel/StrataFlash",
	size: FLASH_SIZE,
	buswidth: FLASH_WIDTH,
	read8: sg_read8,
	read16: sg_read16,
	read32: sg_read32,
	copy_from: sg_copy_from,
	write8: sg_write8,
	write16: sg_write16,
	write32: sg_write32,
	copy_to: sg_copy_to
};

static struct mtd_partition sg_partitions[] = {
	{
		name: "U-Boot Loader",
		offset: 0,
		size: 0x00020000
	},
	{
		name: "OpenGear non-volatile configuration",
		offset: 0x00020000,
		size: 0x001e0000
	},
	{
		name: "OpenGear image",
		offset: 0x200000,
	},
	{
		name: "OpenGear Intel/StrataFlash",
		offset: 0
	},
};

#else
/****************************************************************************/
/*                           SNAPGEAR FLASH                                 */
/****************************************************************************/

#define	VENDOR		"SnapGear"

#if defined(CONFIG_MACH_SE5100)
#define	VENDOR_ROOTFS	"SnapGear filesystem"
#else
#define	VENDOR_ROOTFS	"SnapGear image"
#endif

/*
 *	Intel FLASH setup. This is the only flash device, it is the entire
 *	non-volatile storage (no IDE CF or hard drive or anything else).
 */
static struct map_info sg_map = {
	name: "SnapGear Intel/StrataFlash",
	size: FLASH_SIZE,
	buswidth: FLASH_WIDTH,
	read8: sg_read8,
	read16: sg_read16,
	read32: sg_read32,
	copy_from: sg_copy_from,
	write8: sg_write8,
	write16: sg_write16,
	write32: sg_write32,
	copy_to: sg_copy_to
};

static struct mtd_partition sg_partitions[] = {
#if defined(CONFIG_MACH_IPD)
	{
		name: "SnapGear Boot Loader",
		offset: 0,
		size: 0x00020000
	},
	{
		name: "SnapGear System Data",
		offset: 0x00020000,
		size: 0x00020000
	},
	{
		name: "SnapGear non-volatile configuration",
		offset: 0x00040000,
		size: 0x00020000
	},
	{
		name: "SnapGear image",
		offset: 0x00060000,
	},
#elif defined(CONFIG_MACH_SE5100)
	/*
	 * if you change the names of these,  check the table below
	 * for unlocking the flash as well
	 */
	{
		name: "SnapGear kernel",
		offset: KERNEL_OFFSET,
		size: KERNEL_SIZE,
	},
	{
		name: "SnapGear filesystem",
		offset: KERNEL_OFFSET + KERNEL_SIZE,
		size: ROOTFS_SIZE,
	},
	{
		name: "SnapGear config",
		offset: KERNEL_OFFSET + KERNEL_SIZE + ROOTFS_SIZE,
		size: CONFIG_SIZE
	},
	{
		name: "SnapGear NetGate config",
		offset: KERNEL_OFFSET + KERNEL_SIZE + ROOTFS_SIZE + CONFIG_SIZE,
		size: NG_CONFIG_SIZE
	},
	{
		name: "SnapGear NetGate var",
		offset: KERNEL_OFFSET + KERNEL_SIZE + ROOTFS_SIZE + CONFIG_SIZE + NG_CONFIG_SIZE,
		size: NG_VAR_SIZE
	},
	{
		name: "SnapGear image partition",
		offset: KERNEL_OFFSET,
		size: KERNEL_SIZE + ROOTFS_SIZE,
	},
	{
		name: "SnapGear BIOS config",
		offset: BOOT_SIZE / 2,
		size: BOOT_SIZE / 2,
	},
	{
		name: "SnapGear BIOS",
		offset: 0,
		size: BOOT_SIZE,
	},
	{
		name: "SnapGear Recover",
		offset: RECOVER_OFFSET,
		size: RECOVER_SIZE,
	},
#else
	{
		name: "SnapGear Boot Loader",
		offset: 0,
		size: 0x00020000
	},
	{
		name: "SnapGear non-volatile configuration",
		offset: 0x00020000,
		size: CONFIG_SIZE
	},
	{
		name: "SnapGear image",
		offset: 0x00020000+CONFIG_SIZE,
	},
#endif
	{
		name: "SnapGear Intel/StrataFlash",
		offset: 0
	},
};

/****************************************************************************/
#endif
/****************************************************************************/

#define NUM_PARTITIONS	(sizeof(sg_partitions)/sizeof(sg_partitions[0]))

/****************************************************************************/

/*
 *	Set the Intel flash back to read mode. Sometimes MTD leaves the
 *	flash in status mode, and if you reboot there is no code to
 *	execute (the flash devices do not get a RESET) :-(
 */
static int sg_reboot_notifier(struct notifier_block *nb, unsigned long val, void *v)
{
	struct cfi_private *cfi = sg_map.fldrv_priv;
	int i;

	/* Make sure all FLASH chips are put back into read mode */
	for (i = 0; cfi && i < cfi->numchips; i++) {
		cfi_send_gen_cmd(0xff, 0x55, cfi->chips[i].start, &sg_map,
			cfi, cfi->device_type, NULL);
	}
	return NOTIFY_OK;
}

static struct notifier_block sg_notifier_block = {
	sg_reboot_notifier, NULL, 0
};

/****************************************************************************/

/*
 *	Find the MTD device with the given name.
 */

static int sg_getmtdindex(char *name)
{
	struct mtd_info *mtd;
	int i, index;

	index = -1;
	for (i = 0; (i < MAX_MTD_DEVICES); i++) {
		mtd = get_mtd_device(NULL, i);
		if (mtd) {
			if (strcmp(mtd->name, name) == 0)
				index = mtd->index;
			put_mtd_device(mtd);
			if (index >= 0)
				break;
		}
	}
	return index;
}

/****************************************************************************/

int __init sg_init(void)
{
	int index, rc;

	printk(VENDOR ": MTD flash setup\n");

#if defined(CONFIG_ARCH_IXP425)
	/* Enable flash writes */
	*IXP425_EXP_CS0 |= IXP425_FLASH_WRITABLE;
#endif
#if defined(CONFIG_MACH_SE5100)
	/* Enable full 32MB of flash */
	*IXP425_EXP_CS1 = *IXP425_EXP_CS0;
#endif

	sg_res = request_mem_region(FLASH_ADDR, FLASH_SIZE, VENDOR " FLASH");
	if (sg_res == NULL) {
		printk(VENDOR ": failed memory resource request?\n");
		return -EIO;
	}

	/*
	 *	Map flash into our virtual address space.
	 */
	sg_map.map_priv_1 = (unsigned long) ioremap(FLASH_ADDR, FLASH_SIZE);
	if (!sg_map.map_priv_1) {
		release_mem_region(FLASH_ADDR, FLASH_SIZE);
		sg_res = NULL;
		printk(VENDOR ": failed to ioremap() flash\n");
		return -EIO;
	}

	if ((sg_mtd = do_map_probe("cfi_probe", &sg_map)) == NULL) {
		iounmap((void *)sg_map.map_priv_1);
		release_mem_region(FLASH_ADDR, FLASH_SIZE);
		sg_res = NULL;
		sg_map.map_priv_1 = 0;
		printk(VENDOR ":probe failed\n");
		return -ENXIO;
	}

	printk(KERN_NOTICE VENDOR ": %s device size = %dK\n",
		sg_mtd->name, sg_mtd->size>>10);

	sg_mtd->module = THIS_MODULE;

	register_reboot_notifier(&sg_notifier_block);
	rc = add_mtd_partitions(sg_mtd, sg_partitions, NUM_PARTITIONS);

#if defined(CONFIG_MACH_SE5100)
	/*
	 * By default the P30 flash is locked at boot,  we need to unlock
	 * some of the partitions for them to operate,  which is a little
	 * cumbersome,  though simple in reality.
	 */

	if (rc == 0 && sg_mtd->unlock) {
		int n, p, i, j, printed;
		char *writable_parts[] = {
			/* "SnapGear kernel",      */
			/* "SnapGear filesystem",  */
			"SnapGear config",
			"SnapGear NetGate config",
			"SnapGear NetGate var",
			"SnapGear image partition",
			/* "SnapGear BIOS config", */
			/* "SnapGear BIOS",        */
			/* "SnapGear Recover",     */
			NULL
		};

		for (n = 0; writable_parts[n]; n++) {
			for (p = 0; p < NUM_PARTITIONS; p++)
				if (strcmp(sg_partitions[p].name, writable_parts[n]) == 0)
					break;
			if (p >= NUM_PARTITIONS) {
				printk(VENDOR ": failed to find %s partition\n",
						writable_parts[n]);
				continue;
			}

			printed = 0;
			for (i = 0; i < sg_mtd->numeraseregions; i++) {
				for (j = 0; j < sg_mtd->eraseregions[i].numblocks; j++) {
					/*
					 * check if this sector is within the current region we
					 * are unlocking
					 */
					if (sg_partitions[p].offset > 
							sg_mtd->eraseregions[i].offset +
							j * sg_mtd->eraseregions[i].erasesize
							||
							sg_partitions[p].offset + sg_partitions[p].size <= 
							sg_mtd->eraseregions[i].offset +
							j * sg_mtd->eraseregions[i].erasesize
							)
						continue;

					if (printed == 0) {
						printed = 1;
						printk(VENDOR ": unlocked %s\n", writable_parts[n]);
					}

					sg_mtd->unlock(sg_mtd,
							sg_mtd->eraseregions[i].offset +
								j * sg_mtd->eraseregions[i].erasesize,
								sg_mtd->eraseregions[i].erasesize);
				}
			}
		}
	} else if (rc == 0)
		printk(VENDOR ": no unlock routine\n");
#endif

	if (rc == 0
#ifdef CONFIG_BLK_DEV_INITRD
		&& initrd_start == 0
#endif
			) {
		/* Mark mtd partition as root device */
		index = sg_getmtdindex(VENDOR_ROOTFS);
		if (index >= 0)
			ROOT_DEV = MKDEV(MTD_BLOCK_MAJOR, index);
	}

	return rc;
}

/****************************************************************************/

void __exit sg_cleanup(void)
{
	unregister_reboot_notifier(&sg_notifier_block);
	if (sg_mtd) {
		del_mtd_partitions(sg_mtd);
		map_destroy(sg_mtd);
	}
	if (sg_map.map_priv_1) {
		iounmap((void *)sg_map.map_priv_1);
		sg_map.map_priv_1 = 0;
	}
	if (sg_res) {
		release_mem_region(FLASH_ADDR, FLASH_SIZE);
		sg_res = NULL;
	}
}

/****************************************************************************/

module_init(sg_init);
module_exit(sg_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Greg Ungerer <gerg@snapgear.com>");
MODULE_DESCRIPTION("SnapGear/ARM flash support");

/****************************************************************************/
