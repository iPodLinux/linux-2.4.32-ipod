/*
 * uCbootstrap.h:  uCbootstrap and uCbootloader system call interface
 *
 * (c) Copyright 1999 Rt-Control Inc.
 * (c) Copyright 2001 Arcturus Networks Inc.
 * Maintained by Michael Durrant <mdurrant@arcturusnetworks.com>
 *
 * Release notes:
 * 7 Nov 2001 by mdurrant@arcturusnetworks.com
 *            - moved old uCbootloader.h file from the 
 *              uCdimm platform directory into linux/asm-m68knommu/
 *            - combined the bootstd.h and flash.h file from uCsimm
 *              and uCdimm into a common uCbootloader.h file.
 *
 */

#ifndef __UCBOOTSTRAP_H__
#define __UCBOOTSTRAP_H__

#define NR_BSC 23            /* last used bootloader system call */

#define __BN_reset        0  /* reset and start the bootloader */
#define __BN_test         1  /* tests the system call interface */
#define __BN_exec         2  /* executes a bootloader image */
#define __BN_exit         3  /* terminates a bootloader image */
#define __BN_program      4  /* program FLASH from a chain */
#define __BN_erase        5  /* erase sector(s) of FLASH */
#define __BN_open         6
#define __BN_write        7
#define __BN_read         8
#define __BN_close        9
#define __BN_mmap         10 /* map a file descriptor into memory */
#define __BN_munmap       11 /* remove a file to memory mapping */
#define __BN_gethwaddr    12 /* get the hardware address of my interfaces */
#define __BN_getserialnum 13 /* get the serial number of this board */
#define __BN_getbenv      14 /* get a bootloader envvar */
#define __BN_setbenv      15 /* get a bootloader envvar */
#define __BN_setpmask     16 /* set the protection mask */
#define __BN_readbenv     17 /* read environment variables */
#define __BN_flash_chattr_range		18
#define __BN_flash_erase_range		19
#define __BN_flash_write_range		20
#define __BN_ramload			21 /* load kernel into RAM and exec */
#define __BN_program2      22  /* program second FLASH from a chain */


/* Calling conventions compatible to (uC)linux/68k
 * We use simmilar macros to call into the bootloader as for uClinux
 */

#define __bsc_return(type, res) \
do { \
   if ((unsigned long)(res) >= (unsigned long)(-64)) { \
      /* let errno be a function, preserve res in %d0 */ \
      int __err = -(res); \
      errno = __err; \
      res = -1; \
   } \
   return (type)(res); \
} while (0)

#define _bsc0(type,name) \
type name(void) \
{ \
   register long __res __asm__ ("%d0") = __BN_##name; \
   __asm__ __volatile__ ("trap #2" \
                         : "=g" (__res) \
                         : "0" (__res) \
                         : "%d0"); \
   __bsc_return(type,__res); \
}

#define _bsc1(type,name,atype,a) \
type name(atype a) \
{ \
   register long __res __asm__ ("%d0") = __BN_##name; \
   register long __a __asm__ ("%d1") = (long)a; \
   __asm__ __volatile__ ("trap #2" \
                         : "=g" (__res) \
                         : "0" (__res), "d" (__a) \
                         : "%d0"); \
   __bsc_return(type,__res); \
}

#define _bsc2(type,name,atype,a,btype,b) \
type name(atype a, btype b) \
{ \
   register long __res __asm__ ("%d0") = __BN_##name; \
   register long __a __asm__ ("%d1") = (long)a; \
   register long __b __asm__ ("%d2") = (long)b; \
   __asm__ __volatile__ ("trap #2" \
                         : "=g" (__res) \
                         : "0" (__res), "d" (__a), "d" (__b) \
                         : "%d0"); \
   __bsc_return(type,__res); \
}

#define _bsc3(type,name,atype,a,btype,b,ctype,c) \
type name(atype a, btype b, ctype c) \
{ \
   register long __res __asm__ ("%d0") = __BN_##name; \
   register long __a __asm__ ("%d1") = (long)a; \
   register long __b __asm__ ("%d2") = (long)b; \
   register long __c __asm__ ("%d3") = (long)c; \
   __asm__ __volatile__ ("trap #2" \
                         : "=g" (__res) \
                         : "0" (__res), "d" (__a), "d" (__b), \
                           "d" (__c) \
                         : "%d0"); \
   __bsc_return(type,__res); \
}

#define _bsc4(type,name,atype,a,btype,b,ctype,c,dtype,d) \
type name(atype a, btype b, ctype c, dtype d) \
{ \
   register long __res __asm__ ("%d0") = __BN_##name; \
   register long __a __asm__ ("%d1") = (long)a; \
   register long __b __asm__ ("%d2") = (long)b; \
   register long __c __asm__ ("%d3") = (long)c; \
   register long __d __asm__ ("%d4") = (long)d; \
   __asm__ __volatile__ ("trap #2" \
                         : "=g" (__res) \
                         : "0" (__res), "d" (__a), "d" (__b), \
                           "d" (__c), "d" (__d) \
                         : "%d0"); \
   __bsc_return(type,__res); \
}

#define _bsc5(type,name,atype,a,btype,b,ctype,c,dtype,d,etype,e) \
type name(atype a, btype b, ctype c, dtype d, etype e) \
{ \
   register long __res __asm__ ("%d0") = __BN_##name; \
   register long __a __asm__ ("%d1") = (long)a; \
   register long __b __asm__ ("%d2") = (long)b; \
   register long __c __asm__ ("%d3") = (long)c; \
   register long __d __asm__ ("%d4") = (long)d; \
   register long __e __asm__ ("%d5") = (long)e; \
   __asm__ __volatile__ ("trap #2" \
                         : "=g" (__res) \
                         : "0" (__res), "d" (__a), "d" (__b), \
                           "d" (__c), "d" (__d), "d" (__e) \
                         : "%d0"); \
   __bsc_return(type,__res); \
}

/* Command codes for the flash_command routine */
#define FLASH_SELECT    0       /* no command; just perform the mapping */
#define FLASH_RESET     1       /* reset to read mode */
#define FLASH_READ      1       /* reset to read mode, by any other name */
#define FLASH_AUTOSEL   2       /* autoselect (fake Vid on pin 9) */
#define FLASH_PROG      3       /* program a word */
#define FLASH_CERASE    4       /* chip erase */
#define FLASH_SERASE    5       /* sector erase */
#define FLASH_ESUSPEND  6       /* suspend sector erase */
#define FLASH_ERESUME   7       /* resume sector erase */
#define FLASH_UB        8       /* go into unlock bypass mode */
#define FLASH_UBPROG    9       /* program a word using unlock bypass */
#define FLASH_UBRESET   10      /* reset to read mode from unlock bypass mode */
#define FLASH_LASTCMD   10      /* used for parameter checking */

/* Return codes from flash_status */
#define STATUS_READY    0       /* ready for action */
#define STATUS_BUSY     1       /* operation in progress */
#define STATUS_ERSUSP   2       /* erase suspended */
#define STATUS_TIMEOUT  3       /* operation timed out */
#define STATUS_ERROR    4       /* unclassified but unhappy status */

/* manufacturer ID */
#define AMDPART   	0x01
#define TOSPART		0x98
#define SSTPART		0xbf
#define ATPART		0x1f

/* A list of 4 AMD device ID's - add others as needed */
#define ID_AM29DL800T   0x224A
#define ID_AM29DL800B   0x22CB
#define ID_AM29LV800T   0x22DA
#define ID_AM29LV800B   0x225B
#define ID_AM29LV160B   0x2249
#define ID_AM29LV160T   0x22C4
#define ID_TC58FVT160	0x00C2
#define ID_TC58FVB160	0x0043
#define ID_TC58FVB160X2	0x00430043
#define ID_AM29LV400B   0x22BA
#define ID_AT49BV1614   0x00C0
#define ID_SST39VF160   0x2782
/* definition for arm7tdni */
#define ID_AT27LV1024	0x0000
#define ID_AT29LV1024	0x0001

#define SECTOR_DIRTY   0x01
#define SECTOR_ERASED  0x02
#define SECTOR_PROTECT 0x04

#define PGM_ERASE_FIRST 0x0001
#define PGM_RESET_AFTER 0x0002
#define PGM_EXEC_AFTER  0x0004
#define PGM_HALT_AFTER  0x0008
#define PGM_DEBUG       0x0010

/* an mnode points at 4k pages of data through an offset table. */
typedef struct _memnode {
  int len;
  int *offset;
} mnode_t;


/*
 * uCbootstrap header specification
 */

#if !defined (_UCHEADER_H_)
#define _UCHEADER_H_
#define _UCHEADER_VERSION_ 0.1

typedef struct {
        unsigned char magic[8];     /* magic number "uCimage\0" */
        int           header_size;  /* after which data begins */
        int           data_size;    /* size of image in bytes */
        char          datecode[12]; /* output of 'date -I': "yyyy-mm-dd" */
        unsigned char md5sum[16];   /* binary md5sum of data */
        char          name[128];    /* filename or ID */
        char          padding[84];  /* pad to 256 bytes*/
} uCimage_header;

#define UCHEADER_MAGIC "uCimage\0" /* including one terminating zero */
#endif /* !defined (_UCHEADER_H_) */




#endif /* __UCBOOTSTRAP_H__ */
