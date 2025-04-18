#include "types.h"
#include "param.h"
#include "riscv.h"
#include "fs.h"
#include "spinlock.h"
#include "defs.h"
#include "file.h"
#include "stat.h"

struct devsw devsw[NDEV]; 
struct {
    struct spinlock lock;
    struct file file[NFILE];
} ftable;

void fileinit(void) {
    initlock(&ftable.lock, "ftable");
}