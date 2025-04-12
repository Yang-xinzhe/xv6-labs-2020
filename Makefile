TOOLPREFIX =riscv64-unknown-elf-

CC = $(TOOLPREFIX)gcc
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy

CFLAGS = -Wall -Wextra -O0 -ffreestanding -nostdlib -mcmodel=medany -march=rv64gc -mabi=lp64

LDFLAGS = -T kernel.ld

OBJS = start.o main.o
TARGET_ELF = kernel.elf
TARGET_BIN = kernel.bin

all: $(TARGET_ELF)

$(TARGET_ELF): $(OBJS) kernel.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET_BIN) $(TARGET_ELF)

run: $(TARGET_ELF)
	qemu-system-riscv64 -machine virt -bios none -kernel $(TARGET_ELF) -serial mon:stdio -nographic

.PHONY: all clean run	