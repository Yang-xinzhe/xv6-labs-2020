// File system implementation.  Five layers:
//   + Blocks: allocator for raw disk blocks.
//   + Log: crash recovery for multi-step updates.
//   + Files: inode allocator, reading, writing, metadata.
//   + Directories: inode with special contents (list of other inodes!)
//   + Names: paths like /usr/rtm/xv6/fs.c for convenient naming.
//
// This file contains the low-level file system manipulation
// routines.  The (higher-level) system call implementations
// are in sysfile.c.

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "buf.h"
#include "file.h"

struct {
    struct spinlock lock;
    struct inode inode[NINODE];
} icache;

void iinit(void) {
    int i = 0;

    initlock(&icache.lock, "icache");
    for(i = 0 ; i < NINODE ; i++) {
        initsleeplock(&icache.inode[i].lock, "inode");
    }
}