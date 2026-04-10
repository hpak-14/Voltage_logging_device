#ifndef SPI_H_
#define SPI_H_

#include "stm32f2xx.h"

void spi_DMA_Init(void);

void SPI_DMA_Transmit(uint8_t *data, uint32_t size);
void SPI_DMA_Receive(uint8_t *data, uint32_t size);
void SPI_DMA_TransmitReceive(uint8_t *tx, uint8_t *rx, uint32_t size);

uint8_t finished_transfer(void);
void reset_finished(void);

void cs_low(void);
void cs_high(void);

extern uint8_t SPI_Transfer_Finished;

extern uint16_t rega1;
extern uint16_t rega2;
extern uint16_t rega3;
extern uint16_t rega4;

extern uint8_t taxa[19];
extern uint8_t raxa[19];

#endif