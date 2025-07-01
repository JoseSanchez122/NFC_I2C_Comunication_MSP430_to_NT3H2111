#ifndef NFC_H_
#define NFC_H_

#include <UART.h>
#include <msp430.h> 

#define NT3H2111_ADDR  0x55
#define BLOCK_SIZE 16

typedef enum I2C_ModeEnum{
    IDLE_MODE,
    NACK_MODE,
    TX_REG_ADDRESS_MODE,
    RX_REG_ADDRESS_MODE,
    TX_DATA_MODE,
    RX_DATA_MODE,
    SWITCH_TO_RX_MODE,
    SWITHC_TO_TX_MODE,
    TIMEOUT_MODE
} I2C_Mode;

extern volatile bool FD;
extern volatile uint8_t Message_to_write [BLOCK_SIZE];
extern volatile uint8_t ReceiveBuffer[BLOCK_SIZE];

extern I2C_Mode I2C_Master_WriteReg(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t count);
extern I2C_Mode I2C_Master_ReadReg(uint8_t dev_addr, uint8_t reg_addr, uint8_t count);

#endif /* NFC_H_ */