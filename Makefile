K=kernel

OBJS = \
	$K/entry.o \
	$K/start.o \
	$K/console.o \
	$K/printf.o \
	$K/uart.o \
	$K/spinlock.o \
	$K/main.o \
	$K/proc.o \
	$K/swtch.o \
	$K/trap.o \
	$K/kernelvec.o \


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

LDFLAGS = -T $K/kernel.ld -z max-page-size=4096

TARGET_ELF = $K/kernel

all: $(TARGET_ELF)

$(TARGET_ELF): $(OBJS) $K/kernel.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
	$(OBJDUMP) -S $@ > $K/kernel.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $K/kernel.sym

$K/%.o: $K/%.c $K/defs.h $K/param.h $K/memlayout.h $K/riscv.h $K/types.h
	$(CC) $(CFLAGS) -c -o $@ $<

$K/%.o: $K/%.S
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $K/*.o $K/*.d $K/*.asm $K/*.sym $(TARGET_ELF)

run: $(TARGET_ELF)
	$(QEMU) -machine virt -bios none -kernel $(TARGET_ELF) -m 128M -smp 1 -nographic -serial mon:stdio

debug: $(TARGET_ELF)
	$(QEMU) -machine virt -bios none -kernel $(TARGET_ELF) -m 128M -smp 1 -nographic -serial mon:stdio -S -s

.PHONY: all clean run debug

-include $K/*.d