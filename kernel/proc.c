#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"


// Must be called with interrupts disabled, 
// to prevent race with process being moved
// to a different CPU
int cpuid() {
    int id = r_tp();
    return id;
}