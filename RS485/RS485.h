#include "stm32f2xx_hal.h"
#include "stm32f2xx_it.h"

extern void RS485_Send(uint8_t *data, uint16_t size);
extern void Enable_Transmitter(void);
extern void Enable_Receiver(void);

uint8_t RxData_RS485 = 0;
uint8_t TxData_RS485[8] = {0};

uint16_t crc16(uint8_t *data, uint16_t len);

