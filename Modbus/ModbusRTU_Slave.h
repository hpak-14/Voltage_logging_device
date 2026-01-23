/*
 * Modbus_Lib.h
 *
 *  Created on: Feb 10, 2022
 *      Author: Ümit Can Güveren
 */


#ifndef INC_MODBUSRTU_SLAVE_H_
#define INC_MODBUSRTU_SLAVE_H_

#include "stdbool.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"

enum ModbusFunctions
{
	ReadCoil = 0x01,
	ReadHoldingRegister = 0x03,
	WriteSingleCoil = 0x05,
	WriteSingleRegister = 0x06,
	WriteMultipleCoils = 0x0F,
	WriteMultipleResisters = 0x10,
};
   
#define SLAVEID    1
#define NUMBER_OF_REGISTER 100
#define NUMBER_OF_COIL 100
#define BUFFERSIZE (NUMBER_OF_REGISTER * 2 + 5)

extern uint16_t ModbusRegister[NUMBER_OF_REGISTER];
extern bool ModbusCoil[NUMBER_OF_COIL];


extern uint8_t uartRxData;
extern uint8_t DataCounter;
extern uint8_t RxInterruptFlag;
extern uint8_t uartTimeCounter;
extern uint8_t uartPacketComplatedFlag;
extern char ModbusRx[BUFFERSIZE];

/* Receiver functions */
void uartDataHandler(void);
void uartTimer(void);
void transmitDataMake(char *msg, uint8_t Lenght);


/* Modbus Functions */
void makePacket_01(char *msg, uint8_t Lenght);
void makePacket_03(char *msg, uint8_t Lenght);
void makePacket_05(char *msg, uint8_t Lenght);
void makePacket_06(char *msg, uint8_t Lenght);
void makePacket_15(char *msg, uint8_t Lenght);
void makePacket_16(char *msg, uint8_t Lenght);


void sendMessage(char *msg, uint8_t len);
uint8_t findByte(int16_t NumberOfCoil);
uint16_t MODBUS_CRC16(char *buf, uint8_t len );



#define V_M1 0
#define V_M2 1
#define V_M3 2
#define V_M4 3
#define V_M5 4
#define V_M6 5
#define V_M7 6
#define V_M8 7

#define V_RMS1 8
#define V_RMS2 9
#define V_RMS3 10
#define V_RMS4 11
#define V_RMS5 12
#define V_RMS6 13
#define V_RMS7 14
#define V_RMS8 15





#endif /* INC_MODBUSRTU_SLAVE_H_ */


