#include "kernel/types.h"
#include "user/user.h"

unsigned long read_cycles(void) {
    unsigned long cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    return cycles;
}

int main(void) {
    unsigned long start, end, cycle;
    
    start = read_cycles();  
    printf("Hello World!\n");
    end = read_cycles();
    
    cycle = end - start;
    printf("cycles: %d\n", cycle);
    exit(0);
}