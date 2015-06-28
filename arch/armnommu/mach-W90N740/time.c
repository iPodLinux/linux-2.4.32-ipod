/*
 * time.c  Timer functions for Winbond W90N740
 */

#include <linux/time.h>
#include <linux/timex.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <linux/interrupt.h>

struct irqaction watchdog_irq =
        {
        name: "watchdog"
                ,
        };

unsigned long winbond_gettimeoffset (void)
{
        return 0;
}

void winbond_timer_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
        CSR_WRITE(TISR,2); /* clear TIF0 */
        do_timer(regs);
}

void winbond_watchdog_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
        CSR_WRITE(WTCR, (CSR_READ(WTCR)&0xF7)|0x01);
}
