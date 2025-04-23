K=kernel
U=user

OBJS = \
	$K/entry.o \
	$K/start.o \
	$K/console.o \
	$K/printf.o \
	$K/uart.o \
	$K/kalloc.o \
	$K/spinlock.o \
	$K/string.o \
	$K/main.o \
	$K/vm.o \
	$K/proc.o \
	$K/swtch.o \
	$K/trampoline.o \
	$K/trap.o \
	$K/syscall.o \
	$K/sysproc.o \
	$K/bio.o \
	$K/fs.o \
	$K/log.o \
	$K/sleeplock.o \
	$K/file.o \
	$K/pipe.o \
	$K/exec.o \
	$K/sysfile.o \
	$K/kernelvec.o \
	$K/plic.o \
	$K/virtio_disk.o \


TOOLPREFIX = riscv64-unknown-elf-

QEMU = qemu-system-riscv64

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

# C编译选项
# -Wall: 开启所有警告
# -Werror: 将警告视为错误
# -O: 优化级别（这里使用-O，可以根据需要调整为-O0, -O1等）
# -fno-omit-frame-pointer: 保留帧指针，便于调试
# -ggdb: 生成GDB调试信息
# -MD: 生成依赖文件 (.d)
# -mcmodel=medany: RISC-V代码模型
# -ffreestanding: 用于编译独立程序（如内核）
# -fno-common: 不将未初始化的全局变量放入common块
# -nostdlib: 不链接标准库
# -mno-relax: 禁止链接器松弛优化
# -I.: 将当前目录加入头文件搜索路径
# -fno-stack-protector: 禁用栈保护（xv6不使用）

CFLAGS = -Wall -Werror -O -fno-omit-frame-pointer -ggdb

CFLAGS += -MD
CFLAGS += -mcmodel=medany
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS += -I.
CFLAGS += $(shell $(CC) -fno-stack-pointer -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)

# Disable PIE when possible (for Ubuntu 16.10 toolchain)
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
CFLAGS += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
CFLAGS += -fno-pie -nopie
endif


TARGET_ELF = $K/kernel

LDFLAGS = -z max-page-size=4096
KERNEL_LDFLAGS = -T $K/kernel.ld $(LDFLAGS)
USER_LDFLAGS = $(LDFLAGS)

all: $(TARGET_ELF)

$K/kernel: $(OBJS) $K/kernel.ld $U/initcode
	$(LD) $(KERNEL_LDFLAGS) -o $@ $(OBJS) 
	$(OBJDUMP) -S $@ > $K/kernel.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $K/kernel.sym

$U/initcode: $U/initcode.S
	$(CC) $(CFLAGS) -march=rv64g -nostdinc -I. -Ikernel -c $U/initcode.S -o $U/initcode.o
	$(LD) $(USER_LDFLAGS) -N -e start -Ttext 0 -o $U/initcode.out $U/initcode.o
	$(OBJCOPY) -S -O binary $U/initcode.out $U/initcode
	$(OBJDUMP) -S $U/initcode.o > $U/initcode.asm

mkfs/mkfs: mkfs/mkfs.c $K/fs.h $K/param.h
	gcc -Werror -Wall -I. -o mkfs/mkfs mkfs/mkfs.c

$K/%.o: $K/%.c $K/defs.h $K/param.h $K/memlayout.h $K/riscv.h $K/types.h
	$(CC) $(CFLAGS) -c -o $@ $<

$K/%.o: $K/%.S
	$(CC) $(CFLAGS) -c -o $@ $<


ULIB = $U/ulib.o $U/usys.o $U/printf.o $U/umalloc.o

_%: %.o $(ULIB)
	$(LD) $(USER_LDFLAGS) -N -e main -Ttext 0 -o $@ $^
	$(OBJDUMP) -S $@ > $*.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $*.sym

$U/usys.S : $U/usys.pl
	perl $U/usys.pl > $U/usys.S

$U/usys.o : $U/usys.S
	$(CC) $(CFLAGS) -c -o $U/usys.o $U/usys.S

UPROGS=\
	$U/_init \
	$U/_sh \


fs.img: mkfs/mkfs $(UPROGS)
	mkfs/mkfs fs.img $(UPROGS)

QEMUOPTS = -machine virt -bios none -kernel $K/kernel -m 128M -smp 3 -nographic
QEMUOPTS += -drive file=fs.img,if=none,format=raw,id=x0
QEMUOPTS += -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

clean:
	rm -f $K/*.o $K/*.d $K/*.asm $K/*.sym $(TARGET_ELF)
	rm -f $U/*.o $U/*.d $U/*.asm $U/_*
	rm -f mkfs/mkfs fs.img

run: $(TARGET_ELF) fs.img
	$(QEMU) $(QEMUOPTS)

debug: $(TARGET_ELF) fs.img
	$(QEMU) $(QEMUOPTS) -S -s

.PHONY: all clean run debug

-include $K/*.d