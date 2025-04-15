struct spinlock;
struct context;

// console.c
void            consoleinit(void);
void            consputc(int);

// proc.c
int             cpuid(void);
struct cpu*     mycpu(void);
struct proc*    myproc();
void            sched(void);
void sleep(void *chan, struct spinlock *lk);
void wakeup(void *chan);

// printf.c
void            printf(char*, ...);
void            panic(char*) __attribute__((noreturn));
void            printfinit(void);

// swtch.S
void            swtch(struct context*, struct context*);


// spinlock.c
void            acquire(struct spinlock*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            push_off(void);
void            pop_off(void);


// uart.c
void            uartinit(void);
void            uartputc(int);
void            uartputc_sync(int);