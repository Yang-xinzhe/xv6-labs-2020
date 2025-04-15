#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs

void main() {
    if(cpuid() == 0) {
        consoleinit();
        printfinit();
        printf("\n");
        printf("Yangxinzhe kernel is booting in hart %d \n", cpuid());
        printf("\n");
        kinit();            // physical page allocator  
        started = 1;     
    } else {
        while(started == 0)
            ;
        __sync_synchronize();
        printf("hart %d starting \n", cpuid());
    }
}