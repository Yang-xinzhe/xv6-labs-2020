//
// Console input and output, to the uart.
// Reads are line at a time.
// Implements special input characters:
//   newline -- end of line
//   control-h -- backspace
//   control-u -- kill line
//   control-d -- end of file
//   control-p -- print process list
//

#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "memlayout.h"
#include "riscv.h"
#include "file.h"
#include "defs.h"

#define BACKSPACE 0x100
#define C(x)  ((x)-'@')  // Control-x

//
// send one character to the uart.
// called by printf, and to echo input characters,
// but not from write().
//
void consputc(int c) {
    if(c == BACKSPACE) {
        // if the user typed backspace, overwrite with a space
        uartputc_sync('\b'); uartputc_sync(' '); uartputc_sync('\b');
    } else {
        uartputc_sync(c);
    }
}

struct {
    struct spinlock lock;

    // input
#define INPUT_BUF 128
    char buf[INPUT_BUF];
    uint r; // Read index
    uint w; // Write index
    uint e; // Edit index
} cons;


//
// user write()s to the console go here
int consolewrite(int user_src, uint64 src, int n) {
    int i ;

    acquire(&cons.lock);
    for(i = 0 ; i < n ; i++) {
        char c;
        if(either_copyin(&c, user_src, src + i, 1) == -1)
            break;
        uartputc(c);
    }
    release(&cons.lock);

    return i;
}


//
// the console input interrupt handler.
// uartintr() calls this for input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
void consoleintr(int c) {
    acquire(&cons.lock);

    switch (c) {
    case C('P'):
        procdump();
        break;
    default:
        break;
    }
    release(&cons.lock);
}
void consoleinit(void){
    initlock(&cons.lock, "cons");

    uartinit();
}