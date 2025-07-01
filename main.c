#include <NFC.h>

void delay_ms(unsigned int ms) {
    // Cada milisegundo equivale a 8000 ciclos de reloj a 8 MHz
    while (ms--) {
        __delay_cycles(8000); // 8000 ciclos = 1 ms
    }
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    initClockTo8MHz();
    initGPIO();
    initI2C();
    UART0_INIT();
    __enable_interrupt();

    while(1){
        if(FD){
            FD = false;
            I2C_Master_ReadReg(NT3H2111_ADDR, 1, BLOCK_SIZE);
            uartA0_print_array((uint8_t *)ReceiveBuffer, BLOCK_SIZE);
            delay_ms(1000);
        }

        if(READ_UART0_BUFF("WRITE from MSP430")){
            CLEAR_UART0BUFF();
            I2C_Master_WriteReg(NT3H2111_ADDR, 1, Message_to_write, BLOCK_SIZE);
            delay_ms(1000);
        }
    }
    
	return 0;
}



