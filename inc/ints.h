#ifndef __INTS_H__
#define __INTS_H__

static inline void cause_interrupt()
{
    __asm__ ("int $0x00");
}

static inline void disable_ints(void)
{ __asm__ volatile ("cli" : : : "cc"); }

static inline void enable_ints(void)
{ __asm__ volatile ("sti" : : : "cc"); }

#endif /*__INTS_H__*/
