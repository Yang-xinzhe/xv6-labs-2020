#define UART0_BASE 0x10000000L
#define UART_THR (UART0_BASE + 0x00) // Transmitter Holding Register (Write Only)
#define UART_LSR (UART0_BASE + 0x05) // Line Status Register (read-only)

#define LSR_TX_IDLE (1 << 5) // Transmitter holding register is empty

void uart_putc(char c) {
    while ((*(volatile unsigned char *)UART_LSR & LSR_TX_IDLE) == 0);
    *(volatile unsigned char *)UART_THR = c;
}

void main() {
    uart_putc('$');
    uart_putc('\n');
    
}