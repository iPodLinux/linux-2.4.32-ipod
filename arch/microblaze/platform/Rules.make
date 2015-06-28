#
# arch/microblaze/platform/Rules.make
#
# This file is included by the global makefile so that you can add your own
# platform-specific flags and dependencies.
# This is the generic platform Rules.make
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 2004       Atmark Techno <yashi@atmark-techno.com>
# Copyright (C) 2004       John Williams <jwilliams@itee.uq.edu.au>
# Copyright (C) 1999,2001  Greg Ungerer (gerg@snapgear.com)
# Copyright (C) 1998,1999  D. Jeff Dionne <jeff@uClinux.org>
# Copyright (C) 1998       Kenneth Albanowski <kjahds@kjahds.com>
# Copyright (C) 1994 by Hamish Macdonald
# Copyright (C) 2000  Lineo Inc. (www.lineo.com)

GCC_DIR = $(shell $(CC) -v 2>&1 | grep specs | sed -e 's/.* \(.*\)specs/\1\./')

INCGCC = $(GCC_DIR)/include

ARCH_DIR = arch/$(ARCH)
PLAT_DIR = $(ARCH_DIR)/platform/$(PLATFORM)

CFLAGS += -O2
CFLAGS += -fno-builtin
CFLAGS += -DNO_MM -DNO_FPU -D__ELF__ -DMAGIC_ROM_PTR
CFLAGS += $(UTS_SYSNAME) -D__linux__
CFLAGS += -I$(INCGCC)

ifeq ($(CONFIG_XILINX_MICROBLAZE0_USE_BARREL),1)
   CFLAGS += -mxl-barrel-shift
endif

ifeq ($(CONFIG_XILINX_MICROBLAZE0_USE_DIV),1)
    CFLAGS += -mno-xl-soft-div
endif

# Is the HW multiply instruction available?
# check if C_FAMILY contains spartan3, virtex2 or virtex4
ifneq (,$(findstring virtex2,$(CONFIG_XILINX_MICROBLAZE0_FAMILY)))
    HARD_MULT := y
    CFLAGS += -mno-xl-soft-mul
endif
ifneq (,$(findstring virtex4,$(CONFIG_XILINX_MICROBLAZE0_FAMILY)))
    HARD_MULT := y
    CFLAGS += -mno-xl-soft-mul
endif
ifneq (,$(findstring spartan3,$(CONFIG_XILINX_MICROBLAZE0_FAMILY)))
    HARD_MULT := y
    CFLAGS += -mno-xl-soft-mul
endif

LD_SCRIPT := linux.ld
LINKFLAGS = -T $(LD_SCRIPT) --defsym _STACK_SIZE=0x00

# C library shenanigans.
# mb-gcc 2.95 split some of the libgcc stuff out into the standalone
# libc.a, with different versions of the libs for different HW mul/div/shift
# support.  We don't support this, instead just linking against bog-standard
# standalone libc.a.  This costs us performance.
#
# New compiler (mb-gcc-3.4.x) at least uses libgcc correctly, so we no longer
# have to link against standalone libc.a.  However, we must explicilty point
# the right, optimised version of libgcc.a.  

ifeq ($(GCC_MAJOR),2)
	LIBGCC = $(GCC_DIR)/libgcc.a
	LIBGCC += $(GCC_DIR)/../../../../microblaze/lib/libc.a
else
	# Build up path to correct libgcc.a
	LIBGCC := $(GCC_DIR)

	ifeq ($(CONFIG_XILINX_MICROBLAZE0_USE_BARREL),1)
		LIBGCC := $(LIBGCC)/bs
	endif

	ifeq ($(HARD_MULT),y)
		LIBGCC := $(LIBGCC)/m
	endif

	LIBGCC := $(LIBGCC)/libgcc.a
endif


#kernel linker script is preprocessed first
$(LINUX) : $(LD_SCRIPT)
$(LD_SCRIPT): $(PLAT_DIR)/$(MODEL).ld.in $(PLAT_DIR)/auto-config.in $(TOPDIR)/.config
	$(CPP) -P -x assembler-with-cpp $(AFLAGS) $< > $@

HEAD := $(ARCH_DIR)/platform/$(PLATFORM)/head_$(MODEL).o

SUBDIRS := $(ARCH_DIR)/kernel $(ARCH_DIR)/mm $(ARCH_DIR)/lib \
	   $(ARCH_DIR)/xilinx_ocp \
           $(ARCH_DIR)/platform/$(PLATFORM) $(SUBDIRS)

CORE_FILES := $(ARCH_DIR)/kernel/kernel.o $(ARCH_DIR)/mm/mm.o \
	    $(ARCH_DIR)/xilinx_ocp/xilinx_ocp.o \
            $(ARCH_DIR)/platform/$(PLATFORM)/platform.o $(CORE_FILES)

LIBS += $(ARCH_DIR)/lib/lib.a $(LIBGCC)

MAKEBOOT = $(MAKE) -C $(ARCH_DIR)/boot

archclean:
	@$(MAKEBOOT) clean
	rm -f $(LD_SCRIPT)
	rm -f $(ARCH_DIR)/platform/$(PLATFORM)/microblaze_defs.h

