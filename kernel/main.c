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
        printf("Yangxinzhe kernel is booting\n");
        printf("\n");
    }
}